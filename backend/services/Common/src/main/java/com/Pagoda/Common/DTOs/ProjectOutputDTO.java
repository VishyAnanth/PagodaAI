package com.Pagoda.Common.DTOs;

import java.util.List;

import lombok.*;
import lombok.experimental.Accessors;

import com.Pagoda.Common.Enums.*;

@Getter
@Setter
@AllArgsConstructor
@NoArgsConstructor
@Accessors(chain = true)
public class ProjectOutputDTO {
    private Status status;
    private List<ProjectDTO> projects;
    private List<TagDTO> tags;
    private List<CommitDTO> commits;
    private String message;
}
