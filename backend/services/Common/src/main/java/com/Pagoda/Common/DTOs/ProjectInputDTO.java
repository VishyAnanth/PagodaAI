package com.Pagoda.Common.DTOs;

import java.util.List;

import lombok.*;
import lombok.experimental.Accessors;

@Getter
@Setter
@AllArgsConstructor
@NoArgsConstructor
@Accessors(chain = true)
public class ProjectInputDTO {
    private ProjectDTO project;
    private List<CommitDTO> commits;
    private List<String> viewers;
    private List<String> contributors;
    private Boolean addChanges;
}