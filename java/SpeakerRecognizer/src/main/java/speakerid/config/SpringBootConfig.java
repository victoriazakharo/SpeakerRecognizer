package speakerid.config;

import org.springframework.beans.factory.annotation.Value;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.boot.web.support.SpringBootServletInitializer;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.ComponentScan;
import org.springframework.web.servlet.config.annotation.ResourceHandlerRegistry;
import org.springframework.web.servlet.config.annotation.WebMvcConfigurer;
import org.springframework.web.servlet.config.annotation.WebMvcConfigurerAdapter;

@SpringBootApplication
@ComponentScan(basePackages = "speakerid")
public class SpringBootConfig extends SpringBootServletInitializer {

    @Value("${speakers.directory}")
    public String speakersDirectory;

    public static void main(String args[]) {
        SpringApplication.run(SpringBootConfig.class);
    }

    @Bean
    public WebMvcConfigurer configurer() {
        return new WebMvcConfigurerAdapter() {
            @Override
            public void addResourceHandlers(ResourceHandlerRegistry registry) {
                registry.addResourceHandler("/**").addResourceLocations("classpath:/static/");
                String audioPath = "file:" + speakersDirectory + "audio/";
                registry.addResourceHandler("/audio/**").addResourceLocations(audioPath);
            }
        };
    }
}