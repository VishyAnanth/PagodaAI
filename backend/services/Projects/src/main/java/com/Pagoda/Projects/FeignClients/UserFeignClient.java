package com.Pagoda.Projects.FeignClients;

import java.util.List;
import java.util.Set;
import org.springframework.cloud.openfeign.FeignClient;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.PostMapping;

import com.Pagoda.Common.DTOs.*;

@FeignClient(name = "Authorization")
public interface UserFeignClient {
    @PostMapping("/getDetails/{username}")
    public AuthenticationOutputDTO getUserDetails(@RequestParam String username);

    @PostMapping("/getDetails")
    public List<AuthenticationOutputDTO> getUsersDetails(@RequestParam List<String> usernames);

    @PostMapping("/setDetails")
    public AuthenticationOutputDTO setUserDetails(@RequestBody AuthenticationInputDTO dto);

    @PostMapping("/search")
    public Set<AuthenticationOutputDTO> searchForUser(@RequestBody List<AuthenticationInputDTO> dtos);

    @PostMapping("/validateEnterpriseUser")
    public AuthenticationOutputDTO validateEnterpriseUser(@RequestBody AuthenticationInputDTO dto);
}