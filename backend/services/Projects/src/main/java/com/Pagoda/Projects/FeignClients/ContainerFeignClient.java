package com.Pagoda.Projects.FeignClients;

import com.Pagoda.Common.DTOs.*;

import org.springframework.cloud.openfeign.FeignClient;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;

@FeignClient(name = "Containers")
public interface ContainerFeignClient {
    
    @PostMapping("/createContainer")
    public ContainerOutputDTO createContainer(@RequestBody ContainerInputDTO dto);
}
