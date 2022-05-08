package com.Pagoda.Projects;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RestController;

import com.Pagoda.Common.DTOs.*;

@RestController
public class ProjectsController {

    @Autowired
    ProjectsService projectsService;
    
    @PostMapping("/createProject")
    public ProjectOutputDTO createProject(@RequestBody ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return projectsService.createProject(dto, request, response);
    }

    @PostMapping("/editProject")
    public ProjectOutputDTO editProject(@RequestBody ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return projectsService.editProject(dto, request, response);
    }
    
    @PostMapping("/forkProject")
    public ProjectOutputDTO forkProject(@RequestBody ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return projectsService.forkProject(dto, request, response);
    }

    @PostMapping("/deleteProject")
    public ProjectOutputDTO deleteProject(@RequestBody ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return projectsService.deleteProject(dto, request, response);
    }

    @PostMapping("/retrieveProject")
    public ProjectOutputDTO retrieveProject(@RequestBody ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return projectsService.retrieveProject(dto, request, response);
    }

    @PostMapping("/searchProjects")
    public ProjectOutputDTO searchProjects(@RequestBody ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return projectsService.searchProjects(dto, request, response);
    }

    @PostMapping("/starProject")
    public ProjectOutputDTO starProject(@RequestBody ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return projectsService.starProject(dto, request, response);
    }

    @PostMapping("/getForks")
    public ProjectOutputDTO getForks(@RequestBody ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return projectsService.getForks(dto, request, response);
    }

    @PostMapping("/getTags")
    public ProjectOutputDTO getTags(@RequestBody ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return projectsService.getTags(dto, request, response);
    }

}
