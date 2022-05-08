package com.Pagoda.Projects;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import com.Pagoda.Common.DTOs.ProjectInputDTO;
import com.Pagoda.Common.DTOs.ProjectOutputDTO;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class EnterpriseProjectsController {
    
   @Autowired
    EnterpriseProjectsService enterpriseProjectsService;

    @PostMapping("/createEnterpriseProject")
    public ProjectOutputDTO createEnterpriseProject(ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return enterpriseProjectsService.createEnterpriseProject(dto, request, response);
    }

    @PostMapping("/editEnterpriseProject")
    public ProjectOutputDTO editEnterpriseProject(ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return enterpriseProjectsService.editEnterpriseProject(dto, request, response);
    }

    @PostMapping("/forkEnterpriseProject")
    public ProjectOutputDTO forkEnterpriseProject(ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return enterpriseProjectsService.forkEnterpriseProject(dto, request, response);
    }

    @PostMapping("/deleteEnterpriseProject")
    public ProjectOutputDTO deleteEnterpriseProject(ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return enterpriseProjectsService.deleteEnterpriseProject(dto, request, response);
    }

    @PostMapping("/retrieveEnterpriseProject")
    public ProjectOutputDTO retrieveEnterpriseProject(ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return enterpriseProjectsService.retrieveEnterpriseProject(dto, request, response);
    }

    @PostMapping("/searchEntepriseProjects")
    public ProjectOutputDTO searchEntepriseProjects(ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return enterpriseProjectsService.searchEntepriseProjects(dto, request, response);
    }

    @PostMapping("/starEnterpriseProject")
    public ProjectOutputDTO starEnterpriseProject(ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return enterpriseProjectsService.starEnterpriseProject(dto, request, response);
    }

    @PostMapping("/getEnterpriseForks")
    public ProjectOutputDTO getEnterpriseForks(ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return enterpriseProjectsService.getEnterpriseForks(dto, request, response);
    }

    @PostMapping("/getEnterpriseTags")
    public ProjectOutputDTO getEnterpriseTags(ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return enterpriseProjectsService.getEnterpriseTags(dto, request, response);
    }

}
