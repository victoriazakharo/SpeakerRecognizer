package speakerid.config;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.context.annotation.ComponentScan;

@SpringBootApplication
@ComponentScan(basePackages = "speakerid")
public class SpringBootConfig {
    public static void main(String args[]) {
        SpringApplication.run(SpringBootConfig.class);
    }
}