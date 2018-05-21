package speakerid.util;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.attribute.BasicFileAttributes;
import java.util.Date;
import java.util.Timer;
import java.util.TimerTask;
import java.util.concurrent.TimeUnit;

@Service
public class Cleanup {
    private static long SessionTimeoutSeconds = 1800;
    private static final long CleanupTimeoutMilliseconds = 60000;

    @Autowired private ResourceLoader loader;

    @Value("${speakers.directory}")
    private String speakersDirectory;

    public void start(){
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

    public void finalCleanup() throws IOException {
        SessionTimeoutSeconds = 0;
        cleanupUserData();
    }

    private void cleanupUserData() throws IOException {
        String[] sources = loader.getSources();
        String[] userDirs = new String[]{"data", "exp", "mfcc", "exp/make_mfcc/data"};
        for(String source : sources){
            for(String dir : userDirs){
                File base = new File(String.format("%s%s/%s", speakersDirectory, source, dir));
                if(base.exists()){
                    File[] dirs = base.listFiles(File::isDirectory);
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
}
