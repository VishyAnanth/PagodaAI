package com.Pagoda.Common.DTOs;

import lombok.*;
import lombok.experimental.Accessors;

@Getter
@Setter
@NoArgsConstructor
@AllArgsConstructor
@Accessors(chain = true)
public class StarDTO {
    private Long userId;
    private Long projectId;
}
