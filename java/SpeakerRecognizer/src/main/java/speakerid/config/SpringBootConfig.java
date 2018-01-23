package speakerid.config;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.boot.web.support.SpringBootServletInitializer;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.ComponentScan;
import org.springframework.web.servlet.config.annotation.ResourceHandlerRegistry;
import org.springframework.web.servlet.config.annotation.WebMvcConfigurer;
import org.springframework.web.servlet.config.annotation.WebMvcConfigurerAdapter;

import java.io.File;

@SpringBootApplication
@ComponentScan(basePackages = "speakerid")
public class SpringBootConfig extends SpringBootServletInitializer {
    public static String SpeakersDirectory;
    public static final String SpeakersPath = "speakers.txt";

    public static void main(String args[]) {
        if(args.length != 1){
            handle("Usage: SpeakerRecognizer [resource-path]");
        }
        File file = new File(args[0]);
        if(!file.exists()){
            handle("File " + args[0] + " cannot be found");
        }
        if(!file.isDirectory()){
            handle("File " + args[0] + " is not a directory");
        }
        SpeakersDirectory = args[0];
        SpringApplication.run(SpringBootConfig.class);
    }

    private static void handle(String message) {
        System.err.println(message);
        System.exit(1);
    }

    @Bean
    WebMvcConfigurer configurer() {
        return new WebMvcConfigurerAdapter() {
            @Override
            public void addResourceHandlers(ResourceHandlerRegistry registry) {
                registry.addResourceHandler("/**").addResourceLocations("classpath:/static/");
                String audioPath = "file:" + SpeakersDirectory + "audio/";
                registry.addResourceHandler("/audio/**").addResourceLocations(audioPath);
            }
        };
    }
}