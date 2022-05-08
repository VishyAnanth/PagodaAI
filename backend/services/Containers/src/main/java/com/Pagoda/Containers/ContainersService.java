package com.Pagoda.Containers;

import java.util.UUID;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import com.Pagoda.Common.DTOs.*;
import com.amazonaws.regions.Regions;
import com.amazonaws.services.ecs.AmazonECS;
import com.amazonaws.services.ecs.AmazonECSClientBuilder;
import com.amazonaws.services.ecs.model.CreateClusterRequest;

import org.springframework.stereotype.Service;

@Service
public class ContainersService {
    
    final AmazonECS ecs = AmazonECSClientBuilder.standard().withRegion(Regions.US_WEST_1).build();

    public ContainerOutputDTO createContainer(ContainerInputDTO dto, HttpServletRequest request, HttpServletResponse response) {
        CreateClusterRequest req = new CreateClusterRequest().withClusterName(UUID.randomUUID().toString());
        ecs.createCluster(req);
        return null;
    }

    

}
