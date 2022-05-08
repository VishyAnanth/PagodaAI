package com.Pagoda.Email;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RestController;

import com.Pagoda.Common.DTOs.*;

@RestController
public class EmailController {

    @Autowired
    private EmailService emailService;

    @PostMapping("/sendEmail")
    public EmailOutputDTO sendEmail(@RequestBody EmailInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return emailService.sendEmail(dto, request, response);
    }
}
