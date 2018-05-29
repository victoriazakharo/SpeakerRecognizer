package speakerid.util;

import java.io.IOException;

public class RecordPreprocessor {
    public static final String ExtensionRegex = "[.][^.]+$";

    public static String preprocess(String path) throws IOException, InterruptedException {
        String newPath = path.replaceFirst(ExtensionRegex, "-16k.wav");
        String cmd = String.format("sox %s -c 1 -r 16000 %s silence 1 0.5 0.03%% -1 0.5 0.03%%", path, newPath);
        Process process = Runtime.getRuntime().exec(cmd);
        process.waitFor();
        return process.exitValue() == 0 ? newPath : null;
    }

    public static String preprocessForKaldi(String path) throws IOException, InterruptedException {
        String newPath = preprocess(path);
        String cmd = String.format("sox %s -b 16 %s", newPath, path);
        Process process = Runtime.getRuntime().exec(cmd);
        process.waitFor();
        return process.exitValue() == 0 ? newPath : null;
    }
}
