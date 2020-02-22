function nospam(user, domain)
{
    addr = user + '@' + domain;
    document.write('<a href=\"mailto:' + addr + '\">' + addr + '<\/a>');
}

