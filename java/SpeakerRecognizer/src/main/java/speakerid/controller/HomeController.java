package speakerid.controller;

import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;
import org.springframework.web.context.request.RequestContextHolder;
import org.springframework.web.multipart.MultipartFile;

import javax.annotation.PostConstruct;
import javax.annotation.PreDestroy;
import java.io.*;
import java.net.Socket;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.attribute.BasicFileAttributes;
import java.text.SimpleDateFormat;
import java.util.*;
import java.util.concurrent.TimeUnit;
import java.util.stream.Stream;

@Controller
public class HomeController {
    private Map<String, Map<Integer, String>> speakers = new HashMap<>();
    private Map<String, List<String>> texts = new HashMap<>();
    private static Socket clientSocket;
    private static BufferedReader input;
    private static DataOutputStream output;

    private static final String HOST = "logic";
    private static final int PORT = 1024;
    private static long SessionTimeoutSeconds = 1800;
    private static final long CleanupTimeoutMilliseconds = 60000;

    @Value("${speakers.directory}")
    private String speakersDirectory;

    @Value("${speakers.filename}")
    private String speakersFilename;

    @PostConstruct
    public void init() {
        loadSpeakersAndTexts();
        try {
            while (true) {
                try {
                    clientSocket = new Socket(HOST, PORT);
                    if (clientSocket != null) {
                        System.out.println("Connected to server socket");
                        break;
                    }
                } catch (Exception e) {
                    System.out.println("waiting for server socket...");
                    Thread.sleep(1000);
                }
            }
            input = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
            output = new DataOutputStream(clientSocket.getOutputStream());
            startCleanup();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @PreDestroy
    public void cleanup() {
        try {
            input.close();
            output.close();
            clientSocket.close();
            SessionTimeoutSeconds = 0;
            cleanupUserData();
        } catch (IOException e) {
            System.err.println(e.getMessage());
        }
    }

    @RequestMapping(value = "/getTestRecords", method = RequestMethod.GET)
    @ResponseBody
    public String[] getTestRecords(@RequestParam String source) {
        String foldername = speakersDirectory + source + "/audio/test";
        File testFolder = new File(foldername);
        File[] records = testFolder.listFiles();
        String[] fileNames = new String[records.length];
        for (int i = 0; i < records.length; i++) {
            fileNames[i] = records[i].getName();
        }
        return fileNames;
    }

    @RequestMapping(value = "/getTexts", method = RequestMethod.GET)
    @ResponseBody
    public List<String> getTexts(@RequestParam String lang) {
        return texts.get(lang);
    }

    @RequestMapping(value = "/getSources", method = RequestMethod.GET)
    @ResponseBody
    public String[] getSources() {
        File[] dirs = new File(speakersDirectory).listFiles(File::isDirectory);
        return Arrays.stream(dirs).map(d -> d.getName()).toArray(String[]::new);
    }

    @RequestMapping(value = "/getUserInfo", method = RequestMethod.GET)
    @ResponseBody
    public Map<String, List<String>> getUserInfo() {
        String id = RequestContextHolder.currentRequestAttributes().getSessionId();
        String[] sources = getSources();
        Map<String, List<String>> enrolledRecords = new HashMap<>();
        for(String source : sources){
            File directory = new File(String.format("%s%s/data/%s", speakersDirectory, source, id));
            if (!directory.exists()) {
                directory.mkdir();
            }
            File[] records = directory.listFiles();
            List<String> recordNames = new ArrayList<>();
            for(File record : records){
                String name = record.getName();
                String extensionRegex = "[.][^.]+$";
                recordNames.add(name.replaceFirst(extensionRegex, ""));
            }
            enrolledRecords.put(source, recordNames);
        }
        return enrolledRecords;
    }

    @RequestMapping(value = "/process", method = RequestMethod.POST)
    @ResponseBody
    public String process(@RequestParam String source, @RequestParam("data") MultipartFile multipartFile)
            throws IOException, InterruptedException {
        String userId = RequestContextHolder.currentRequestAttributes().getSessionId();
        String name = multipartFile.getOriginalFilename();
        String userDataDir = String.format("%s%s/data/%s/", speakersDirectory, source, userId);
        String recordPath = userDataDir + name;
        File uploadedFile = new File(recordPath);
        multipartFile.transferTo(uploadedFile);
        recordPath = preprocess(recordPath);
        File recordFile = new File(recordPath);

        String extensionRegex = "[.][^.]+$";
        String recordId = name.replaceFirst(extensionRegex, "");

        File text = new File(userDataDir + "text");
        String sourceLang = source.split("_")[0];
        String textContent = String.format("%s %s\n", recordId, texts.get(sourceLang).get(Integer.parseInt(recordId)));
        try(Writer out = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(text), "UTF8"))){
            out.write(textContent);
        }
        Path spk2utt = Paths.get(userDataDir + "spk2utt");
        Files.write(spk2utt, String.format("0 %s\n", recordId).getBytes());

        Path utt2spk = Paths.get(userDataDir + "utt2spk");
        Files.write(utt2spk, String.format("%s 0\n", recordId).getBytes());

        Path wavscp = Paths.get(userDataDir + "wav.scp");
        String relativeName = String.format("data/%s/%s", userId, recordFile.getName());
        Files.write(wavscp, String.format("%s %s", recordId, relativeName).getBytes());

        Process process = new ProcessBuilder("/kaldi/process.sh",
                "/kaldi", speakersDirectory + source, userId, recordId).inheritIO().start();
        int result = process.waitFor();
        uploadedFile.delete();
        recordFile.delete();
        return result == 0 ? "accepted" : "failed";
    }

    @RequestMapping(value = "/getSpeakerRecords", method = RequestMethod.GET)
    @ResponseBody
    public String[][] getSpeakerRecords(@RequestParam String source) {
        String foldername = speakersDirectory + source + "/audio/example";
        File testFolder = new File(foldername);
        File[] records = testFolder.listFiles();
        String[][] result = new String[records.length][2];
        String extensionRegex = "[.][^.]+$";
        for (int i = 0; i < records.length; i++) {
            String filename = records[i].getName();
            String speaker = filename.replaceFirst(extensionRegex, "");
            result[i] = new String[2];
            result[i][0] = filename;
            result[i][1] = speakers.get(source).get(Integer.valueOf(speaker));
        }
        return result;
    }

    @RequestMapping(value = "/recognize", method = RequestMethod.GET)
    @ResponseBody
    public String recognize(@RequestParam String source, @RequestParam String path) {
        String result = "not found";
        try {
            output.writeBytes(String.format("%s %s%s\n", source, speakersDirectory, path));
            int speaker = Integer.valueOf(input.readLine());
            result = speakers.get(source).get(speaker);
        } catch (Exception e) {
            e.printStackTrace();
        }
        return result;
    }

    @RequestMapping(value = "/recognizeUploaded", method = RequestMethod.POST)
    @ResponseBody
    public String recognizeRecorded(@RequestParam("source") String source,
                                    @RequestParam("data") MultipartFile multipartFile) {
        String time = new SimpleDateFormat("HH-mm-ss.SSS").format(new Date());
        String path = String.format("%s%d_%s.wav", speakersDirectory, Thread.currentThread().getId(), time);
        String result = null;
        try {
            File uploadedFile = new File(path);
            multipartFile.transferTo(uploadedFile);
            path = preprocess(path);
            uploadedFile.delete();
            output.writeBytes(String.format("%s %s\n", source, path));
            int speaker = Integer.valueOf(input.readLine());
            result = speakers.get(source).get(speaker);
            new File(path).delete();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return result;
    }

    private void loadSpeakersAndTexts() {
        File speakersDir = new File(speakersDirectory);
        File[] dirs = speakersDir.listFiles(File::isDirectory);
        for(File dir : dirs) {
            String source = dir.getName();
            loadTexts(source);
            Map<Integer, String> map = new HashMap<>();
            String path = String.format("%s%s/%s",speakersDirectory, source, speakersFilename);
            try (Stream<String> stream = Files.lines(Paths.get(path), Charset.forName("UTF-16"))) {
                stream.forEach(
                        line -> {
                            String[] words = line.split(" ");
                            map.putIfAbsent(Integer.valueOf(words[0]), words[1]);
                        }
                );
                speakers.put(source, map);
            } catch (Exception e) {
                e.printStackTrace();
            }
            System.out.printf("Loaded %d speakers from %s\n", map.size(), source);
        }
    }

    private void loadTexts(String source){
        String sourceLang = source.split("_")[0];
        if(!texts.containsKey(sourceLang)) {
            List<String> lines = new ArrayList<>();
            File file = new File(String.format("%s%s/texts",speakersDirectory, source));
            try (Scanner scanner = new Scanner(file, "UTF-8")) {
                while(scanner.hasNextLine()){
                    lines.add(scanner.nextLine());
                }
                scanner.close();
                texts.put(sourceLang, lines);
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    private String preprocess(String path) throws IOException, InterruptedException {
        String extensionRegex = "[.][^.]+$";
        String newPath = path.replaceFirst(extensionRegex, "-16k.wav");
        //String cmd = "cp /kaldi/1.wav " + newPath;
        String cmd = String.format("sox %s -c 1 -r 16000 %s silence 1 0.5 0.03%% -1 0.5 0.03%%", path, newPath);
        Process process = Runtime.getRuntime().exec(cmd);
        process.waitFor();
        return newPath;
    }

    private void startCleanup(){
        Timer timer = new Timer();
        timer.schedule(new TimerTask() {
            @Override
            public void run() {
                try {
                    cleanupUserData();
                } catch (IOException e){
                    e.printStackTrace();
                }
            }
        }, 0, CleanupTimeoutMilliseconds);
    }

    private void cleanupUserData() throws IOException {
        String[] sources = getSources();
        String[] userDirs = new String[]{"data", "exp", "mfcc"};
        for(String source : sources){
            for(String dir : userDirs){
                String path = String.format("%s%s/%s", speakersDirectory, source, dir);
                File[] dirs = new File(path).listFiles(File::isDirectory);
                for(File directory : dirs) {
                    if(directory.getName() != "lang"){
                        BasicFileAttributes attr = Files.readAttributes(directory.toPath(), BasicFileAttributes.class);
                        long created = attr.creationTime().to(TimeUnit.SECONDS);
                        long now = new Date().getTime() / 1000;
                        if(now - created > SessionTimeoutSeconds){
                            directory.delete();
                        }
                    }
                }
            }
        }
    }
}
