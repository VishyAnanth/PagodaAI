package com.Pagoda.Projects;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import com.Pagoda.Common.DTOs.*;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;

public class EnterpriseVersionControlController {
    
    @Autowired
    EnterpriseVersionControlService enterpriseVersionControlService;

    @PostMapping("/commitEnterprise")
    public ProjectOutputDTO commitEnterprise(ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return enterpriseVersionControlService.commitEnterprise(dto, request, response);
    }

    @PostMapping("/branchEnterprise")
    public ProjectOutputDTO branchEnterprise(ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return enterpriseVersionControlService.branchEnterprise(dto, request, response);
    }

    @PostMapping("/mergeEnterprise")
    public ProjectOutputDTO mergeEnterprise(ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return enterpriseVersionControlService.mergeEnterprise(dto, request, response);
    }

    @PostMapping("/retrieveCommitEnterprise")
    public ProjectOutputDTO retrieveCommitEnterprise(ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return enterpriseVersionControlService.retrieveCommitEnterprise(dto, request, response);
    }

    @PostMapping("/retrieveCommitsEnterprise")
    public ProjectOutputDTO retrieveCommitsEnterprise(ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return enterpriseVersionControlService.retrieveCommitsEnterprise(dto, request, response);
    }

}
