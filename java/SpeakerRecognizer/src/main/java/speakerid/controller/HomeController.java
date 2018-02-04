package speakerid.controller;

import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;
import org.springframework.web.multipart.MultipartFile;

import javax.annotation.PostConstruct;
import javax.annotation.PreDestroy;
import java.io.*;
import java.net.Socket;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.HashMap;
import java.util.Map;
import java.util.stream.Stream;

import static speakerid.config.SpringBootConfig.SpeakersDirectory;
import static speakerid.config.SpringBootConfig.SpeakersPath;

@Controller
public class HomeController {
    private Map<Integer, String> speakers = new HashMap<>();
    private static Socket clientSocket;
    private static BufferedReader input;
    private static DataOutputStream output;

    private static final String HOST = "logic";
    private static final int PORT = 1024;

    private void loadSpeakers() {
        try (Stream<String> stream = Files.lines(Paths.get(SpeakersDirectory + SpeakersPath))) {
            stream.forEach(
                    line -> {
                        String[] words = line.split(" ");
                        speakers.putIfAbsent(Integer.valueOf(words[0]), words[1]);
                    }
            );
        } catch (Exception e) {
            e.printStackTrace();
        }
        System.out.printf("Loaded %d speakers\n", speakers.size());
    }

    @PostConstruct
    public void init() {
        loadSpeakers();
        try {
            while (true) {
                try {
                    clientSocket = new Socket(HOST, PORT);
                    if (clientSocket != null) {
                        System.out.println("Connected to server socket");
                        break;
                    }
                }
                catch (Exception e) {
                    System.out.println("waiting for server socket...");
                    Thread.sleep(1000);
                }
            }
            input = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
            output = new DataOutputStream(clientSocket.getOutputStream());
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
        } catch (IOException e) {
            System.err.println(e.getMessage());
        }
    }

    @RequestMapping(value = "/getTestRecords", method = RequestMethod.GET)
    @ResponseBody
    public String[] getTestRecords(){
        File testFolder = new File(SpeakersDirectory + "audio/test");
        File[] records = testFolder.listFiles();
        String[] fileNames = new String[records.length];
        for(int i = 0; i < records.length; i++){
            fileNames[i] = records[i].getName();
        }
        return fileNames;
    }

    @RequestMapping(value = "/getSpeakerRecords", method = RequestMethod.GET)
    @ResponseBody
    public String[][] getSpeakerRecords(){
        File testFolder = new File(SpeakersDirectory + "audio/example");
        File[] records = testFolder.listFiles();
        String[][] result = new String[records.length][2];
        String extensionRegex = "[.][^.]+$";
        for(int i = 0; i < records.length; i++){
            String filename = records[i].getName();
            String speaker = filename.replaceFirst(extensionRegex, "");
            String[] record = new String[2];
            record[0] = filename;
            record[1] = speakers.get(Integer.valueOf(speaker));
            result[i] = record;
        }
        return result;
    }

    @RequestMapping(value = "/recognize", method = RequestMethod.GET)
    @ResponseBody
    public String recognize(@RequestParam String path) {
        String result = "not found";
        try {
            output.writeBytes(SpeakersDirectory + path + "\n");
            int speaker = Integer.valueOf(input.readLine());
            result = speakers.get(speaker);
        } catch (IOException e) {
            e.printStackTrace();
        }
        return result;
    }

    @RequestMapping(value = "/recognizeUploaded", method = RequestMethod.POST)
    @ResponseBody
    public String recognizeRecorded(@RequestParam("data") MultipartFile multipartFile) {
        String path = SpeakersDirectory + "uploaded.wav";
        String result = null;
        try {
            File uploadedFile = new File(path);
            multipartFile.transferTo(uploadedFile);
            output.writeBytes(path + "\n");
            int speaker = Integer.valueOf(input.readLine());
            result = speakers.get(speaker);
        } catch (Exception e) {
            e.printStackTrace();
        }
        return result;
    }
}
