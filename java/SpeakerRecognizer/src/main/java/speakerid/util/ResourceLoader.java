package speakerid.util;

import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;

import java.io.File;
import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.*;
import java.util.stream.Stream;

@Service
public class ResourceLoader {
    @Value("${speakers.directory}")
    private String speakersDirectory;

    @Value("${speakers.filename}")
    private String speakersFilename;

    public String[] getSources() {
        File[] dirs = new File(speakersDirectory).listFiles(File::isDirectory);
        return Arrays.stream(dirs).map(d -> d.getName()).toArray(String[]::new);
    }

    public void load(Map<String, Map<Integer, String>> speakers, Map<String, List<String>> texts) {
        File speakersDir = new File(speakersDirectory);
        File[] dirs = speakersDir.listFiles(File::isDirectory);
        for(File dir : dirs) {
            String source = dir.getName();
            loadTexts(texts, source);
            Map<Integer, String> map = new HashMap<>();
            String path = String.format("%s%s/%s", speakersDirectory, source, speakersFilename);
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

    private void loadTexts(Map<String, List<String>> texts, String source){
        String sourceLang = source.split("_")[0];
        if(!texts.containsKey(sourceLang)) {
            List<String> lines = new ArrayList<>();
            File file = new File(String.format("%s%s/texts", speakersDirectory, source));
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
}
