package com.Pagoda.Common.DTOs;

import lombok.*;
import lombok.experimental.Accessors;

@Getter
@Setter
@AllArgsConstructor
@NoArgsConstructor
@Accessors(chain = true)
public class ForkDTO {
    private Long forkId;
    private Long forkedProjectId;
}
