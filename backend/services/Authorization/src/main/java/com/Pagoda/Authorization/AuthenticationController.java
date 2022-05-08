package com.Pagoda.Authorization;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.RestController;

import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.GetMapping;

import com.Pagoda.Common.DTOs.*;


//will need an enterprise controller with all this shit in it, maybe a separate enterprise microservice that DNS forwards requests to
//(ie. piedpiper.pagoda.com -> enterprise microservice, but users -> this microservice)
//also need to figure out how to do third party saml auth and enterprise service on prem for enterprise customers
@RestController
public class AuthenticationController {
    
    @Autowired
	AuthenticationService authenticationService;

    @PostMapping("/signUp")
    public AuthenticationOutputDTO signUp(@RequestBody AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return authenticationService.signUp(dto, request, response);
    }

    @PostMapping("/signIn")
    public AuthenticationOutputDTO signIn(@RequestBody AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return authenticationService.signIn(dto, request, response);
    }

    @PostMapping("/signOut")
    public AuthenticationOutputDTO signOut(@RequestBody AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return authenticationService.signOut(dto, request, response);
    }

    @GetMapping("/verifyUser")
    public AuthenticationOutputDTO verifyUser(@RequestParam String id, @RequestParam String token, HttpServletRequest request, HttpServletResponse response) {
        return authenticationService.verifyUser(id, token, request, response);
    }

    @PostMapping("/forgotPassword")
    public AuthenticationOutputDTO forgotPassword(@RequestBody AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return authenticationService.forgotPassword(dto, request, response);
    }

    @GetMapping("/passwordToken")
    public AuthenticationOutputDTO passwordToken(@RequestParam String id, @RequestParam String token, HttpServletRequest request, HttpServletResponse response) {
        return authenticationService.passwordToken(id, token, request, response);
    }

    @PostMapping("/resetPassword")
    public AuthenticationOutputDTO resetPassword(@RequestBody AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return authenticationService.resetPassword(dto, request, response);
    }

}