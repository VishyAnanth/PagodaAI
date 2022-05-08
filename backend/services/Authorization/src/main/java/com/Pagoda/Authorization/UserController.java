package com.Pagoda.Authorization;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import java.util.List;
import java.util.Set;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestParam;

import com.Pagoda.Common.DTOs.*;

//REMEMBER TO REMOVE ID FROM RESPONSE BODY IN ZUUL!!!
@RestController
public class UserController {

    @Autowired
    UserService userService;

    @PostMapping("/getDetails/{username}")
    public AuthenticationOutputDTO getUserDetails(@RequestParam String username, HttpServletRequest request, HttpServletResponse response) {
        return userService.getUserDetails(username, request, response);
    }

    @PostMapping("/getDetails")
    public List<AuthenticationOutputDTO> getUsersDetails(@RequestParam List<String> usernames, HttpServletRequest request, HttpServletResponse response) {
        return userService.getUsersDetails(usernames, request, response);
    }

    @PostMapping("/setDetails") //dont let users change username, password, or boolean shit
    public AuthenticationOutputDTO setUserDetails(@RequestBody AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return userService.setUserDetails(dto, request, response);
    }

    @PostMapping("/search")
    public Set<AuthenticationOutputDTO> searchForUser(@RequestBody List<AuthenticationInputDTO> dtos, HttpServletRequest request, HttpServletResponse response) {
        return userService.searchForUser(dtos, request, response);
    }

    @PostMapping("/getEnterprises")
    public List<AuthenticationOutputDTO> getEnterprisesForUser(@RequestBody AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return userService.getEnterprisesForUser(dto, request, response);
    }

}