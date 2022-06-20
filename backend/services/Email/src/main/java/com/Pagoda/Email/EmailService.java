package com.Pagoda.Email;

import java.io.File;
import java.util.Properties;

import javax.mail.MessagingException;
import javax.mail.internet.MimeMessage;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import com.Pagoda.Common.DTOs.EmailInputDTO;
import com.Pagoda.Common.DTOs.EmailOutputDTO;
import com.Pagoda.Common.Enums.*;
import com.Pagoda.Email.DetailsServices.EmailDetails;
import com.Pagoda.Email.DetailsServices.EmailDetailsService;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Bean;
import org.springframework.core.io.FileSystemResource;
import org.springframework.mail.javamail.JavaMailSender;
import org.springframework.mail.javamail.JavaMailSenderImpl;
import org.springframework.mail.javamail.MimeMessageHelper;
import org.springframework.stereotype.Service;
import org.apache.commons.validator.routines.EmailValidator;

@Service
public class EmailService {
    
    @Bean
    public JavaMailSender getJavaMailSender() {
        JavaMailSenderImpl mailSender = new JavaMailSenderImpl();
        mailSender.setHost("smtp.gmail.com");
        mailSender.setPort(587);
        
        mailSender.setUsername(senderEmail);
        mailSender.setPassword(senderPassword);
        
        Properties props = mailSender.getJavaMailProperties();
        props.put("mail.transport.protocol", "smtp");
        props.put("mail.smtp.auth", "true");
        props.put("mail.smtp.starttls.enable", "true");
        props.put("mail.debug", "true");
        
        return mailSender;
    }
    
    @Autowired
    private EmailDetailsService emailDetailsService;

    @Value("${sender.email}")
    private String senderEmail;

    @Value("${sender.password}")
    private String senderPassword;

    public EmailOutputDTO sendEmail(EmailInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        EmailOutputDTO res = new EmailOutputDTO()
        .setStatus(Status.SUCCESS);
        
        if(!EmailValidator.getInstance().isValid(senderEmail) ||
        !EmailValidator.getInstance().isValid(dto.getRecipient())) {
            return res.setStatus(Status.ERROR);
        }

        EmailDetails details = emailDetailsService.loadEmailDetailsById(dto.getEmailFormatId());
        if(details == null) {
            return res.setStatus(Status.ERROR);
        }
        JavaMailSender emailSender = getJavaMailSender();

        MimeMessage message = emailSender.createMimeMessage();

        try {
            MimeMessageHelper helper = new MimeMessageHelper(message, true);
    
            helper.setFrom(senderEmail);
            helper.setTo(dto.getRecipient());
            helper.setSubject(details.getEmailFormat().getSubject());
            helper.setText(details.getEmailFormat().getContents());
    
            dto.getEmailAttachments().forEach((key, value) -> 
            {
                try {
                    helper.addAttachment(key, new FileSystemResource(new File(value)));
                } catch (MessagingException e) {}
            }
            );
        } catch(MessagingException e) {
            return res.setStatus(Status.ERROR);
        }
        
        emailSender.send(message);

        return res;
    }

}
