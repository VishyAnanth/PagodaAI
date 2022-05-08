package com.Pagoda.Gateway.Config;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Configuration;
import org.springframework.cloud.gateway.route.RouteLocator;
import org.springframework.context.annotation.Bean;
import org.springframework.cloud.gateway.route.builder.RouteLocatorBuilder;

import com.Pagoda.Gateway.Filters.*;

@Configuration
public class GatewayConfig {

    @Autowired
    private JwtAuthFilter jwtAuthFilter;

    @Autowired BlockFilter blockFilter;

    //replace all these filters with the legit ones
    @Bean
	public RouteLocator routes(RouteLocatorBuilder builder) {
		return builder.routes().route("Authorization", r -> r.path("/" + System.getenv("AUTHORIZATION_SERVICE_PATH") + "/**").filters(f -> f.filter(jwtAuthFilter)).uri("lb://" + System.getenv("AUTHORIZATION_SERVICE_ID")))
				.route("Projects", r -> r.path("/" + System.getenv("PROJECT_SERVICE_PATH") + "/**").filters(f -> f.filter(jwtAuthFilter)).uri("lb://" + System.getenv("PROJECT_SERVICE_ID")))
				.route("Editor", r -> r.path("/" + System.getenv("EDITOR_SERVICE_PATH") + "/**").filters(f -> f.filter(jwtAuthFilter)).uri("lb://" + System.getenv("EDITOR_SERVICE_ID")))
				.route("Training", r -> r.path("/" + System.getenv("TRAINING_SERVICE_PATH") + "/**").filters(f -> f.filter(jwtAuthFilter)).uri("lb://" + System.getenv("TRAINING_SERVICE_ID")))
                .route("Inference", r -> r.path("/" + System.getenv("INFERENCE_SERVICE_PATH") + "/**").filters(f -> f.filter(jwtAuthFilter)).uri("lb://" + System.getenv("INFERENCE_SERVICE_ID")))
                .route("Logging", r -> r.path("/" + System.getenv("LOGGING_SERVICE_PATH") + "/**").filters(f -> f.filter(jwtAuthFilter)).uri("lb://" + System.getenv("LOGGING_SERVICE_ID")))
                .route("VersionControl", r -> r.path("/" + System.getenv("VERSION_CONTROL_SERVICE_PATH") + "/**").filters(f -> f.filter(jwtAuthFilter)).uri("lb://"+ System.getenv("VERSION_CONTROL_SERVICE_ID")))
                .route("Containers", r -> r.path("/" + System.getenv("CONTAINER_SERVICE_PATH") + "/**").filters(f -> f.filter(jwtAuthFilter)).uri("lb://" + System.getenv("CONTAINER_SERVICE_ID")))
                .route("Payments", r -> r.path("/" + System.getenv("PAYMENTS_SERVICE_PATH") + "/**").filters(f -> f.filter(jwtAuthFilter)).uri("lb://" + System.getenv("PAYMENTS_SERVICE_ID")))
                .route("Email", r -> r.path("/"+ System.getenv("PAYMENTS_SERVICE_PATH") + "/**").filters(f -> f.filter(blockFilter)).uri("lb://" + System.getenv("EMAIL_SERVICE_ID")))
                .build();
	}
}