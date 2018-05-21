package speakerid.util;

import java.io.IOException;

public class RecordPreprocessor {
    public static String preprocess(String path) throws IOException, InterruptedException {
        String extensionRegex = "[.][^.]+$";
        String newPath = path.replaceFirst(extensionRegex, "-16k.wav");
        String cmd = String.format("sox %s -c 1 -r 16000 %s silence 1 0.5 0.03%% -1 0.5 0.03%%", path, newPath);
        Process process = Runtime.getRuntime().exec(cmd);
        process.waitFor();
        return newPath;
    }
}
