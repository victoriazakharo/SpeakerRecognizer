package speakerid.controller;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;
import org.springframework.web.context.request.RequestContextHolder;
import org.springframework.web.multipart.MultipartFile;
import speakerid.util.Cleanup;
import speakerid.util.RecordPreprocessor;
import speakerid.util.ResourceLoader;

import javax.annotation.PostConstruct;
import javax.annotation.PreDestroy;
import java.io.*;
import java.net.Socket;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.text.SimpleDateFormat;
import java.util.*;

@Controller
public class HomeController {
    private Map<String, Map<Integer, String>> speakers = new HashMap<>();
    private Map<String, List<String>> texts = new HashMap<>();

    private static Socket clientSocket;
    private static BufferedReader input;
    private static DataOutputStream output;

    private static final String HOST = "logic";
    private static final int PORT = 1024;
    private static final String USER_DICTOR_NUMBER = "30";

    @Autowired private ResourceLoader loader;
    @Autowired private Cleanup cleanup;

    @Value("${speakers.directory}")
    private String speakersDirectory;

    @Value("${kaldi.directory}")
    private String kaldiDirectory;

    @Value("${speakers.filename}")
    private String speakersFilename;

    @PostConstruct
    public void init() {
        loader.load(speakers, texts);
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
            new ProcessBuilder(kaldiDirectory + "/link_kaldi_tools.sh",
                    kaldiDirectory).inheritIO().start().waitFor();
            cleanup.start();
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
            cleanup.finalCleanup();
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
        return loader.getSources();
    }

    @RequestMapping(value = "/getUserInfo", method = RequestMethod.GET)
    @ResponseBody
    public Map<String, List<String>> getUserInfo() {
        String[] sources = loader.getSources();
        Map<String, List<String>> enrolledRecords = new HashMap<>();
        for(String source : sources){
            String userDir = getUserDirectory(source);
            File directory = new File(userDir);
            if (!directory.exists()) {
                directory.mkdir();
                new File(userDir + "/models").mkdir();
            }
            File[] files = directory.listFiles();
            List<String> recordNames = new ArrayList<>();
            for(File file : files){
                String name = file.getName();
                if(name.startsWith("ali.")){
                    recordNames.add(name.substring(4));
                }
            }
            enrolledRecords.put(source, recordNames);
        }
        return enrolledRecords;
    }

    @RequestMapping(value = "/process", method = RequestMethod.POST)
    @ResponseBody
    public String process(@RequestParam String source, @RequestParam("data") MultipartFile multipartFile)
            throws IOException, InterruptedException {
        String userId = getUserId();
        String name = multipartFile.getOriginalFilename();
        String userDataDir = getUserDirectory(source) + "/";
        String recordPath = userDataDir + name;
        File uploadedFile = new File(recordPath);
        multipartFile.transferTo(uploadedFile);
        if(RecordPreprocessor.preprocessForKaldi(recordPath) == null){
            return "failed";
        }
        String recordId = name.replaceFirst(RecordPreprocessor.ExtensionRegex, "");

        File text = new File(userDataDir + "text");
        String textContent = String.format("%s %s\n", recordId, texts.get(getLanguage(source)).get(Integer.parseInt(recordId)));
        try(Writer out = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(text), "UTF8"))){
            out.write(textContent);
        }
        Path spk2utt = Paths.get(userDataDir + "spk2utt");
        Files.write(spk2utt, String.format("%s %s\n", USER_DICTOR_NUMBER, recordId).getBytes());

        Path utt2spk = Paths.get(userDataDir + "utt2spk");
        Files.write(utt2spk, String.format("%s %s\n", recordId, USER_DICTOR_NUMBER).getBytes());

        Path wavscp = Paths.get(userDataDir + "wav.scp");
        String relativeName = String.format("data/%s/%s", userId, name);
        Files.write(wavscp, String.format("%s %s", recordId, relativeName).getBytes());

        Process process = new ProcessBuilder(kaldiDirectory + "/process.sh",
                kaldiDirectory, speakersDirectory + source, userId, recordId).inheritIO().start();
        int result = process.waitFor();
        return result == 0 ? "accepted" : "failed";
    }

    @RequestMapping(value = "/getSpeakerRecords", method = RequestMethod.GET)
    @ResponseBody
    public String[][] getSpeakerRecords(@RequestParam String source) {
        String foldername = speakersDirectory + source + "/audio/example";
        File testFolder = new File(foldername);
        File[] records = testFolder.listFiles();
        String[][] result = new String[records.length][2];
        for (int i = 0; i < records.length; i++) {
            String filename = records[i].getName();
            String speaker = filename.replaceFirst(RecordPreprocessor.ExtensionRegex, "");
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
            String command = getRecognizeCommand(source, speakersDirectory + path);
            output.writeBytes(command);
            String answer = input.readLine();
            result = getSpeakerName(answer, source);
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
        String name = multipartFile.getOriginalFilename();
        String extension = name.substring(name.lastIndexOf('.'));
        String path = String.format("%s%d_%s%s", speakersDirectory, Thread.currentThread().getId(), time, extension);
        String result = "not found";
        try {
            File uploadedFile = new File(path);
            multipartFile.transferTo(uploadedFile);
            path = RecordPreprocessor.preprocess(path);
            if(path == null){
                return result;
            }
            uploadedFile.delete();
            String command = getRecognizeCommand(source, path);
            output.writeBytes(command);
            String answer = input.readLine();
            result = getSpeakerName(answer, source);
            new File(path).delete();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return result;
    }

    @RequestMapping(value = "/add", method = RequestMethod.GET)
    @ResponseBody
    public String add(@RequestParam String source) throws IOException, InterruptedException {
        int numberOfRecords = texts.get(getLanguage(source)).size();
        String userDir = getUserDirectory(source);
        output.writeBytes(String.format("%s %s/ %s\n", source, userDir, numberOfRecords));
        return input.readLine();
    }

    private String getUserId(){
        return RequestContextHolder.currentRequestAttributes().getSessionId();
    }

    private String getLanguage(String source){
        return source.split("_")[0];
    }

    private String getUserDirectory(String source){
        return String.format("%s%s/data/%s", speakersDirectory, source, getUserId());
    }

    private String getSpeakerName(String answer, String source){
        return answer.equals(USER_DICTOR_NUMBER) ?
                "you" :
                speakers.get(source).get(Integer.valueOf(answer));
    }

    private String getRecognizeCommand(String source, String path){
        return String.format("%s %s%s/data/%s/ %s\n", source, speakersDirectory, source, getUserId(), path);
    }
}
