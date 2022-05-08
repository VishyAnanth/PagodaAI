package com.Pagoda.Projects;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import com.Pagoda.Common.DTOs.*;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class VersionControlController {
    
    @Autowired
    VersionControlService versionControlService;

    @PostMapping("/commit")
    public ProjectOutputDTO commit(@RequestBody ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return versionControlService.commit(dto, request, response);
    }

    @PostMapping("/branch")
    public ProjectOutputDTO branch(@RequestBody ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return versionControlService.branch(dto, request, response);
    }

    @PostMapping("/merge")
    public ProjectOutputDTO merge(@RequestBody ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return versionControlService.merge(dto, request, response);
    }

    @PostMapping("/retrieveCommit")
    public ProjectOutputDTO retrieveCommit(@RequestBody ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return versionControlService.retrieveCommit(dto, request, response);
    }

    @PostMapping("/retrieveCommits")
    public ProjectOutputDTO retrieveCommits(@RequestBody ProjectInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return versionControlService.retrieveCommits(dto, request, response);
    }

}
