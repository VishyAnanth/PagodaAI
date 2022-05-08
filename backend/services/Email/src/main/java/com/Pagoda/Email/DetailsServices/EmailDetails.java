package com.Pagoda.Email.DetailsServices;

import com.Pagoda.Common.DTOs.EmailFormatDTO;

public class EmailDetails {
    private EmailFormatDTO emailFormat;

    public EmailDetails(EmailFormatDTO emailFormat) {
        this.emailFormat = emailFormat;
    }

    public EmailDetails build(EmailFormatDTO emailFormat) {
        return new EmailDetails(emailFormat);
    }

    public EmailFormatDTO getEmailFormat() {
        return this.emailFormat;
    }
}
