package com.Pagoda.Gateway.Utils;

import org.springframework.stereotype.Component;
import org.springframework.beans.factory.annotation.Value;
import io.jsonwebtoken.*;

import com.Pagoda.Common.Exceptions.*;

@Component
public class JwtUtils {
    @Value("${jwt.secret}")
    private String jwtSecret;

    public Claims getClaims(final String token) {
        try {
			Claims body = Jwts.parser().setSigningKey(jwtSecret).parseClaimsJws(token).getBody();
			return body;
		} catch (Exception e) {
			//Log error here
		}
		return null;
    }

    public void validateToken(final String token) throws JwtTokenMalformedException, JwtTokenMissingException {
        try {
            Jwts.parser().setSigningKey(jwtSecret).parseClaimsJws(token);
        } catch (SignatureException ex) {
			throw new JwtTokenMalformedException("Signature Exception");
		} catch (MalformedJwtException ex) {
			throw new JwtTokenMalformedException("Malformed JWT Exception");
		} catch (ExpiredJwtException ex) {
			throw new JwtTokenMalformedException("Expired JWT Exception");
		} catch (UnsupportedJwtException ex) {
			throw new JwtTokenMalformedException("Unsupported JWT Exception");
		} catch (IllegalArgumentException ex) {
			throw new JwtTokenMissingException("Illegal Arugment Exception");
		}
    }
}