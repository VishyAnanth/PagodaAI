package com.Pagoda.Authorization.Security;

import org.springframework.stereotype.Component;
import org.springframework.beans.factory.annotation.Value;
import io.jsonwebtoken.*;
import org.springframework.security.core.Authentication;
import java.util.Date;
import org.springframework.security.core.GrantedAuthority;
import java.util.stream.Collectors;
import java.nio.charset.Charset;

import com.Pagoda.Authorization.DetailsServices.UserDetailsAuthImplementation;

@Component
public class JwtUtils {

    @Value("${jwt.secret}")
    private String jwtSecret;

    @Value("${jwt.validity}")
    private long jwtValidity;

    public Claims getClaims(final String token) {
        try {
			Claims body = Jwts.parser().setSigningKey(jwtSecret).parseClaimsJws(token).getBody();
			return body;
		} catch (Exception e) {
			//Log error here
		}
		return null;
    }

    public String generateToken(Authentication authentication) {
        UserDetailsAuthImplementation userPrincipal = (UserDetailsAuthImplementation) authentication.getPrincipal();
        return Jwts.builder().claim("authorities", authentication.getAuthorities().stream()
        .map(GrantedAuthority::getAuthority).collect(Collectors.toList())).setSubject((userPrincipal.getUsername())).setIssuedAt(new Date(System.currentTimeMillis()))
            .setExpiration(new Date(System.currentTimeMillis() + jwtValidity)).signWith(SignatureAlgorithm.HS512, jwtSecret.getBytes(Charset.forName("UTF-8")))
            .compact();
    }

    public String generateEnterpriseToken(Authentication authentication, String enterpriseName) {
        UserDetailsAuthImplementation userPrincipal = (UserDetailsAuthImplementation) authentication.getPrincipal();
        return Jwts.builder().claim("authorities", authentication.getAuthorities().stream()
        .map(GrantedAuthority::getAuthority).collect(Collectors.toList())).setSubject((userPrincipal.getUsername() + "," + enterpriseName)).setIssuedAt(new Date(System.currentTimeMillis()))
            .setExpiration(new Date(System.currentTimeMillis() + jwtValidity)).signWith(SignatureAlgorithm.HS512, jwtSecret.getBytes(Charset.forName("UTF-8")))
            .compact();
    }
}