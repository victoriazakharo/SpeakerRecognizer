package speakerid.controller;

import org.springframework.beans.factory.annotation.Value;
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
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.text.SimpleDateFormat;
import java.util.Arrays;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;
import java.util.stream.Stream;

@Controller
public class HomeController {
    private Map<String, Map<Integer, String>> speakers = new HashMap<>();
    private static Socket clientSocket;
    private static BufferedReader input;
    private static DataOutputStream output;


    private static final String HOST = "logic";
    private static final int PORT = 1024;

    @Value("${speakers.directory}")
    private String speakersDirectory;

    @Value("${speakers.filename}")
    private String speakersFilename;

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
                } catch (Exception e) {
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

    @RequestMapping(value = "/getSources", method = RequestMethod.GET)
    @ResponseBody
    public String[] getSources() {
        File[] dirs = new File(speakersDirectory).listFiles(File::isDirectory);
        return Arrays.stream(dirs).map(d -> d.getName()).toArray(String[]::new);
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

    private void loadSpeakers() {
        File[] dirs = new File(speakersDirectory).listFiles(File::isDirectory);
        for(File dir : dirs) {
            Map<Integer, String> map = new HashMap<>();
            String source = dir.getName();
            String path = speakersDirectory + source + "/" + speakersFilename;
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

    private String preprocess(String path) throws IOException, InterruptedException {
        String extensionRegex = "[.][^.]+$";
        String newPath = path.replaceFirst(extensionRegex, "-16k.wav");
        String cmd = String.format("sox %s -c 1 -r 16000 %s silence 1 0.01 0.03%% -1 0.01 0.03%%", path, newPath);
        Process process = Runtime.getRuntime().exec(cmd);
        process.waitFor();
        return newPath;
    }
}
