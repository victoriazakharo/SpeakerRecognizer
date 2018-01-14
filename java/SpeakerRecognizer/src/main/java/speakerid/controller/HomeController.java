package speakerid.controller;

import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.ResponseBody;

import javax.annotation.PostConstruct;
import javax.annotation.PreDestroy;
import java.io.*;
import java.net.Socket;
import java.net.URL;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.*;
import java.util.stream.Stream;

@Controller
public class HomeController {
    private Map<Integer, String> dictors;
    private static Socket clientSocket;
    private static BufferedReader input;
    private static DataOutputStream output;

    private static final String HOST = "localhost";
    private static final int PORT = 1024;
    private static String root;

    private void loadDictors(){
        dictors = new HashMap<Integer, String>();
        URL dictorsFile = HomeController.class.getResource("/dictors.txt");
        try (Stream<String> stream = Files.lines(Paths.get(dictorsFile.toURI()))) {
            stream.forEach(
                    line -> {
                        String[] words = line.split(" ");
                        dictors.putIfAbsent(Integer.valueOf(words[0]), words[1]);
                    }
            );
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @PostConstruct
    public void init() {
        loadDictors();
        try {
            root = (this.getClass().getClassLoader().getResource("").getPath() + "../../").substring(1);
            clientSocket = new Socket(HOST, PORT);
            input = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
            output = new DataOutputStream(clientSocket.getOutputStream());
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @PreDestroy
    public void cleanup(){
        try {
            input.close();
            output.close();
            clientSocket.close();
        } catch (IOException e) {
            System.err.println(e.getMessage());
        }
    }

    @RequestMapping(value = "/", method = RequestMethod.GET)
    public String index() {
        return "index";
    }

    @RequestMapping(value = "/recognize", method = RequestMethod.GET)
    @ResponseBody
    public String recognize(@RequestParam String path) {
        String result = "not found";
        try {
            output.writeBytes(root + path + "\n");
            int dictor = Integer.valueOf(input.readLine());
            result = dictors.get(dictor);
        } catch (IOException e) {
            e.printStackTrace();
        }
        return result;
    }
}