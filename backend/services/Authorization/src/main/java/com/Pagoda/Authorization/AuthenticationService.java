package com.Pagoda.Authorization;

import java.security.SecureRandom;
import java.util.Date;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import com.Pagoda.Authorization.DetailsServices.RolesService;
import com.Pagoda.Authorization.DetailsServices.UserDetailsAuthImplementation;
import com.Pagoda.Authorization.DetailsServices.UserDetailsAuthService;
import com.Pagoda.Authorization.FeignClients.EmailFeignClient;
import com.Pagoda.Authorization.Security.*;
import com.Pagoda.Common.DTOs.*;
import com.Pagoda.Common.Enums.*;

import org.apache.commons.validator.routines.EmailValidator;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.security.authentication.AuthenticationManager;
import org.springframework.security.authentication.UsernamePasswordAuthenticationToken;
import org.springframework.security.core.Authentication;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.security.crypto.argon2.Argon2PasswordEncoder;
import org.springframework.security.crypto.password.PasswordEncoder;
import org.springframework.security.web.authentication.logout.SecurityContextLogoutHandler;
import org.springframework.stereotype.Service;

@Service
public class AuthenticationService {

    @Autowired
	private AuthenticationManager authenticationManager;

    @Autowired
	private JwtUtils jwtUtils;

    @Autowired
    private UserDetailsAuthService userDetailsService;

    @Autowired
    private RolesService rolesService;

    @Autowired
    private EmailFeignClient emailFeignClient;

    public Boolean validAuth() {
        Authentication auth = SecurityContextHolder.getContext().getAuthentication();
        return auth != null;
    }

    public PasswordEncoder passwordEncoder() {
		return new Argon2PasswordEncoder(16, 32, 4, 1048576, 2);
	}

    public AuthenticationOutputDTO signUp(AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        AuthenticationOutputDTO res = new AuthenticationOutputDTO()
        .setStatus(Status.SUCCESS);
        if(((UserDetailsAuthImplementation)userDetailsService.loadUserByUsername(dto.getUser().getUsername())).getUser() != null) {
            return res.setStatus(Status.ERROR).setMessage("Username already in use.");
        }
        if(((UserDetailsAuthImplementation)userDetailsService.loadUserByEmail(dto.getUser().getEmail())).getUser() != null) {
            return res.setStatus(Status.ERROR).setMessage("Email already in use.");
        }
        if(dto.getUser().getUsername().length() == 0 || dto.getUser().getPassword().length() == 0 ||
        dto.getUser().getEmail().length() == 0 || dto.getUser().getFirstName().length() == 0 ||
        dto.getUser().getLastName().length() == 0 || !EmailValidator.getInstance().isValid(dto.getUser().getEmail()) 
        || dto.getUser().getUsername().contains(",")) {
            return res.setStatus(Status.ERROR).setMessage("Invalid fields");
        }
        dto.getUser().setPassword(passwordEncoder().encode(dto.getUser().getPassword()))
        .setId(new SecureRandom().nextLong())
        .setToken(String.valueOf(new SecureRandom().nextLong()))
        .setTokenExpiration(new Date(System.currentTimeMillis() + 86400000));
        userDetailsService.createUser(dto.getUser());

        //format email correctly
        emailFeignClient.sendEmail(new EmailInputDTO().setRecipient(dto.getUser().getEmail())
        .setEmailFormatId(Long.valueOf(-1)));

        rolesService.createRole(new UserRoleDTO()
        .setEnterpriseId(Long.valueOf(1))
        .setId(new SecureRandom().nextLong())
        .setRoleId(1)
        .setUserId(dto.getUser().getId())); //Pagpda is by default 1, role id needs to be determined

        return res.setUser(dto.getUser().unsetPassword().unsetId());
    }
    
    public AuthenticationOutputDTO signIn(AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        AuthenticationOutputDTO res = new AuthenticationOutputDTO()
        .setStatus(Status.SUCCESS);
        UserDetailsAuthImplementation details = (UserDetailsAuthImplementation)userDetailsService.loadUserByUsername(dto.getUser().getUsername());
        if(!details.getUser().getEnabled()) {
            return res.setStatus(Status.ERROR).setMessage("Unverified account");
        }
        Authentication authentication = authenticationManager.authenticate(new UsernamePasswordAuthenticationToken(dto.getUser().getUsername(), dto.getUser().getPassword()));
        SecurityContextHolder.getContext().setAuthentication(authentication);
        String jwt = jwtUtils.generateToken(authentication);
        UserDetailsAuthImplementation userDetails = (UserDetailsAuthImplementation) authentication.getPrincipal();
        userDetailsService.editUser(userDetails.getUser().setToken(jwt));
        res.setUser(userDetails.getUser().unsetPassword().unsetId()).setToken(jwt);
        
        return res;
    }
    
    public AuthenticationOutputDTO signOut(AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        AuthenticationOutputDTO res = new AuthenticationOutputDTO()
        .setStatus(Status.SUCCESS);
        if(validAuth()) {
            UserDetailsAuthImplementation details = (UserDetailsAuthImplementation) SecurityContextHolder.getContext().getAuthentication();
            userDetailsService.editUser(details.getUser().setToken(""));
            new SecurityContextLogoutHandler().logout(request, response, SecurityContextHolder.getContext().getAuthentication());
            return res;
        }
        return res.setStatus(Status.ERROR).setMessage("Invalid JWT");
    }

    public AuthenticationOutputDTO verifyUser(String id, String token, HttpServletRequest request, HttpServletResponse response) {
        AuthenticationOutputDTO res = new AuthenticationOutputDTO()
        .setStatus(Status.SUCCESS);
        UserDetailsAuthImplementation details = (UserDetailsAuthImplementation)userDetailsService.loadUserById(id);
        if(details.getUser() == null) {
            return res.setStatus(Status.ERROR).setMessage("User not found");
        }
        if(details.getUser().getEnabled()) {
            return res.setStatus(Status.ERROR).setMessage("Invalid token");
        }
        if(!details.getUser().getToken().equals(token) || details.getUser().getTokenExpiration().before(new Date(System.currentTimeMillis())) ) {
            userDetailsService.deleteUser(details.getUser());
            return res.setStatus(Status.ERROR).setMessage("Invalid token");
        }
        details.getUser().setToken("").setEnabled(true);
        userDetailsService.editUser(details.getUser());
        return res.setUser(details.getUser().unsetPassword().unsetId());
    }

    public AuthenticationOutputDTO forgotPassword(AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        AuthenticationOutputDTO res = new AuthenticationOutputDTO()
        .setStatus(Status.SUCCESS);
        UserDetailsAuthImplementation details = (UserDetailsAuthImplementation)userDetailsService.loadUserByEmail(dto.getUser().getEmail());
        if(details.getUser() == null) {
            return res.setStatus(Status.ERROR).setMessage("User not found");
        }
        if(!details.getUser().getToken().equals("") && !details.getUser().getEnabled()) {
            return res.setStatus(Status.ERROR).setMessage("Please verify your account");
        }
        details.getUser().setToken(String.valueOf(new SecureRandom().nextLong())).setTokenExpiration(new Date(System.currentTimeMillis() + 3600000));
        userDetailsService.editUser(details.getUser());
        
        emailFeignClient.sendEmail(new EmailInputDTO().setRecipient(details.getUser().getEmail())
        .setEmailFormatId(Long.valueOf(-1)));//FIX THIS

        return res.setUser(details.getUser().unsetPassword().unsetId()).setMessage("Email Sent");
    }

    public AuthenticationOutputDTO passwordToken(String id, String token, HttpServletRequest request, HttpServletResponse response) {
        AuthenticationOutputDTO res = new AuthenticationOutputDTO()
        .setStatus(Status.SUCCESS);
        UserDetailsAuthImplementation details = (UserDetailsAuthImplementation)userDetailsService.loadUserById(id);
       
        if(details.getUser() == null) {
            return res.setStatus(Status.ERROR).setMessage("User not found");
        }
        if(!details.getUser().getEnabled() || !details.getUser().getNonExpired() || !details.getUser().getNonLocked() || !details.getUser().getCredentialsNonExpired() ||
            !details.getUser().getToken().equals(token) || details.getUser().getTokenExpiration().before(new Date(System.currentTimeMillis()))) {
                details.getUser().setToken("");
                userDetailsService.editUser(details.getUser());
                return res.setStatus(Status.ERROR).setMessage("Invalid token");
        }

        return res.setUser(details.getUser().unsetPassword());
    }

    public AuthenticationOutputDTO resetPassword(AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        AuthenticationOutputDTO res = new AuthenticationOutputDTO()
        .setStatus(Status.SUCCESS);

        UserDetailsAuthImplementation details = (UserDetailsAuthImplementation)userDetailsService.loadUserByUsername(dto.getUser().getUsername());
        if(details.getUser() == null) {
            return res.setStatus(Status.ERROR).setMessage("User not found");
        }
        if(details.getUser().getToken().equals("") || !dto.getUser().getToken().equals(details.getUser().getToken()) || !dto.getUser().getTokenExpiration().equals(details.getUser().getTokenExpiration()) ||
            details.getUser().getTokenExpiration().before(new Date(System.currentTimeMillis()))) {
                details.getUser().setToken("");
                userDetailsService.editUser(details.getUser());
                return res.setStatus(Status.ERROR).setMessage("Invalid token");
        }
        details.getUser().setPassword(passwordEncoder().encode(dto.getUser().getPassword())).setToken("");
        userDetailsService.editUser(details.getUser());
        return res.setUser(details.getUser().unsetPassword().unsetId());
    }
}
