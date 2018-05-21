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

    @Autowired private ResourceLoader loader;
    @Autowired private Cleanup cleanup;

    @Value("${speakers.directory}")
    private String speakersDirectory;

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
        String id = RequestContextHolder.currentRequestAttributes().getSessionId();
        String[] sources = loader.getSources();
        Map<String, List<String>> enrolledRecords = new HashMap<>();
        for(String source : sources){
            String userDir = String.format("%s%s/data/%s", speakersDirectory, source, id);
            File directory = new File(userDir);
            if (!directory.exists()) {
                directory.mkdir();
                new File(userDir + "/models").mkdir();
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
        recordPath = RecordPreprocessor.preprocess(recordPath);
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
            path = RecordPreprocessor.preprocess(path);
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
}
