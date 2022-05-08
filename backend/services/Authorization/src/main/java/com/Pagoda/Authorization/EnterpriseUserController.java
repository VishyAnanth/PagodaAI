package com.Pagoda.Authorization;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import com.Pagoda.Common.DTOs.AuthenticationInputDTO;
import com.Pagoda.Common.DTOs.AuthenticationOutputDTO;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class EnterpriseUserController {
    
    @Autowired
    EnterpriseUserService enterpriseUserService;

    @PostMapping("/createEnterprise")
    public AuthenticationOutputDTO createEnterprise(@RequestBody AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return enterpriseUserService.createEnterprise(dto, request, response);
    }

    @PostMapping("/signInToEnterprise")
    public AuthenticationOutputDTO signInToEnterprise(@RequestBody AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return enterpriseUserService.signInToEnterprise(dto, request, response);
    }

    @PostMapping("/signOutOfEnterprise")
    public AuthenticationOutputDTO signOutOfEnterprise(@RequestBody AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return enterpriseUserService.signOutOfEnterprise(dto, request, response);
    }

    @PostMapping("/addUserToEnterprise")
    public AuthenticationOutputDTO addUserToEnterprise(@RequestBody AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return enterpriseUserService.addUserToEnterprise(dto, request, response);
    }

    @PostMapping("/inviteExternalEmailToEnterprise")
    public AuthenticationOutputDTO inviteExternalEmailToEnterprise(@RequestBody AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return enterpriseUserService.inviteExternalEmailToEnterprise(dto, request, response);
    }

    @PostMapping("/acceptInvitationExternal")
    public AuthenticationOutputDTO acceptInvitationExternal(@RequestBody AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return enterpriseUserService.acceptInvitationExternal(dto, request, response);
    }

    @PostMapping("/declineInvitationExternal")
    public AuthenticationOutputDTO declineInvitationExternal(@RequestBody AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return enterpriseUserService.declineInvitationExternal(dto, request, response);
    }

    @PostMapping("/addRoleToEnterprise")
    public AuthenticationOutputDTO addRoleToEnterprise(@RequestBody AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return enterpriseUserService.addRoleToEnterprise(dto, request, response);
    }
    
    @PostMapping("/removeUserFromEnterprise")
    public AuthenticationOutputDTO removeUserFromEnterprise(@RequestBody AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return enterpriseUserService.removeUserFromEnterprise(dto, request, response);
    }

    @PostMapping("/removeRoleFromEnterprise")
    public AuthenticationOutputDTO removeRoleFromEnterprise(@RequestBody AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return enterpriseUserService.removeRoleFromEnterprise(dto, request, response);
    }

    @PostMapping("/validateEnterpriseUser")
    public AuthenticationOutputDTO validateEnterpriseUser(@RequestBody AuthenticationInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return enterpriseUserService.validateEnterpriseUser(dto, request, response);
    }

}
