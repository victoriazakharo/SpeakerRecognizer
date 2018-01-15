package speakerid.controller;

import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;

import javax.annotation.PostConstruct;
import javax.annotation.PreDestroy;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.HashMap;
import java.util.Map;
import java.util.stream.Stream;

@Controller
public class HomeController {
    private Map<Integer, String> speakers = new HashMap<>();
    private static Socket clientSocket;
    private static BufferedReader input;
    private static DataOutputStream output;

    private static final String HOST = "localhost";
    private static final int PORT = 1024;

    //TODO: rename according to business logic
    @Value("${file.name}")
    private String fileName;

    @Value("#{systemProperties['os.name'].contains('Windows') ? '${windows.speakers.directory}' : '${unix.speakers.directory}'}")
    private String speakersDirectory;

    private void loadSpeakers() {
        try (Stream<String> stream = Files.lines(Paths.get(speakersDirectory + fileName))) {
            stream.forEach(
                    line -> {
                        String[] words = line.split(" ");
                        speakers.putIfAbsent(Integer.valueOf(words[0]), words[1]);
                    }
            );
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @PostConstruct
    public void init() {
        loadSpeakers();
        try {
            clientSocket = new Socket(HOST, PORT);
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

    @RequestMapping(value = "/recognize", method = RequestMethod.GET)
    @ResponseBody
    public String recognize(@RequestParam String path) {
        String result = "not found";
        try {
            output.writeBytes(speakersDirectory + path + "\n");
            int speaker = Integer.valueOf(input.readLine());
            result = speakers.get(speaker);
        } catch (IOException e) {
            e.printStackTrace();
        }
        return result;
    }
}