package com.Pagoda.Containers;

import com.Pagoda.Common.DTOs.*;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;

@Controller
public class ContainersController {
    
    @Autowired
    ContainersService containersService;

    //This is basically only going to be used by enterprise customers to isolate evironment
    @PostMapping("/createContainer")
    public ContainerOutputDTO createContainer(@RequestBody ContainerInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        return containersService.createContainer(dto, request, response);
    }

    

}
