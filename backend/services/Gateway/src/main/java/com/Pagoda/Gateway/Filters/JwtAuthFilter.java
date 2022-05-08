package com.Pagoda.Gateway.Filters;

import java.util.List;
import java.util.function.Predicate;
import java.util.stream.Collectors;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.cloud.gateway.filter.GatewayFilter;
import org.springframework.cloud.gateway.filter.GatewayFilterChain;
import org.springframework.http.HttpStatus;
import org.springframework.http.server.reactive.ServerHttpRequest;
import org.springframework.http.server.reactive.ServerHttpResponse;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.stereotype.Component;
import org.springframework.web.server.ServerWebExchange;
import org.springframework.security.core.authority.SimpleGrantedAuthority;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;

import io.jsonwebtoken.Claims;
import reactor.core.publisher.Mono;

import com.Pagoda.Gateway.Utils.*;
import com.Pagoda.Common.Exceptions.*;

@Component
public class JwtAuthFilter implements GatewayFilter {

    @Autowired
    private JwtUtils jwtUtils;

    @Override
    public Mono<Void> filter(ServerWebExchange exchange, GatewayFilterChain chain) {
        ServerHttpRequest request = (ServerHttpRequest) exchange.getRequest();
        
        final List<String> allowedPaths = GeneralUtils.envVars.stream().map(env -> System.getenv(env)).collect(Collectors.toList());

        Predicate<ServerHttpRequest> isApiSecured = r -> allowedPaths.stream().noneMatch(uri -> r.getURI().getPath().contains(uri));

        if (isApiSecured.test(request)) {
            if (!request.getHeaders().containsKey("Authorization")) {
				ServerHttpResponse response = exchange.getResponse();
				response.setStatusCode(HttpStatus.UNAUTHORIZED);
				return response.setComplete();
			}

			final String token = request.getHeaders().getOrEmpty("Authorization").get(0);

			try {
				jwtUtils.validateToken(token);
			} catch (JwtTokenMalformedException | JwtTokenMissingException e) {
				ServerHttpResponse response = exchange.getResponse();
				response.setStatusCode(HttpStatus.BAD_REQUEST);
				return response.setComplete();
			}

			Claims claims = jwtUtils.getClaims(token);
            if(claims != null) {
                String username = claims.getSubject();
                List<String> auths = (List<String>)claims.get("authorities");
                UsernamePasswordAuthenticationToken auth = new UsernamePasswordAuthenticationToken(username, null, auths.stream()
                    .map(SimpleGrantedAuthority::new)
                    .collect(Collectors.toList()));
                auth.setDetails(token);
                SecurityContextHolder.getContext().setAuthentication(auth);
            }
        }
        return chain.filter(exchange);
    }
}