package com.Pagoda.Authorization.FeignClients;

import com.Pagoda.Common.DTOs.*;

import org.springframework.cloud.openfeign.FeignClient;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;

@FeignClient(name = "Email")
public interface EmailFeignClient {

    @PostMapping("/sendEmail")
    public EmailOutputDTO sendEmail(@RequestBody EmailInputDTO dto);
    
}
