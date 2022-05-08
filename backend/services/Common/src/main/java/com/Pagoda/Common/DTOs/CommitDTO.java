package com.Pagoda.Common.DTOs;

import java.util.Date;
import java.util.List;
import lombok.*;
import lombok.experimental.Accessors;

@Getter
@Setter
@AllArgsConstructor
@NoArgsConstructor
@Accessors(chain = true)
public class CommitDTO {
    private String commitHash;
    List<String> parents;
    private Long authorId;
    private String description;
    private Date commitDate;
    private String treeId;
    private Long projectId;
    //figure out how to encode data
}
