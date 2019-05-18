
static bool f__SessionSendData(SOCKET client, const char *data, int32_t dsz)
{
    do
    {
        int rsz;

        if (send(client, data, dsz, 0) == -1)
            break;

        auto buf = std::make_unique<char[]>(256);
        if ((rsz = recv(client, buf.get(), 256, 0)) < 4)
            break;

        if (memcmp(
                buf.get(),
                DriverConst::ls_const_fail,
                4U) == 0)
            break;

        return true;
    }
    while (0);

    return false;
}

static bool f__SessionSendTarget(SOCKET client, std::wstring const & dev)
{
    std::string target = f__MakeTarget<std::string>(dev);
    std::string cmds = f__MakeCmd<std::string>(target);
    return f__SessionSendData(client, cmds.c_str(), cmds.length());
}

static bool f__SessionSendCmd(SOCKET client, std::string const & data)
{
    std::string cmds = f__MakeCmd<std::string>(data);
    return f__SessionSendData(client, cmds.c_str(), cmds.length());
}

static bool f__SessionSendSunc(
    SOCKET client,
    std::string const & dst, std::string const & cmd, std::string const & fname
    )
{
    do
    {
        int32_t rsz = 0;

        if (!f__SessionSendCmd(client, cmd))
            break;

        /// ADB sync SEND protocol
        DriverNet::ADBSYNCDATA req = { {'S','E','N','D'}, dst.length() };

        if (
            (send(client, (const char*)&req, sizeof(req), 0) < static_cast<int32_t>(sizeof(req))) ||
            (send(client, dst.data(), dst.length(), 0) < static_cast<int32_t>(dst.length()))
           )
            break;

        auto dbuf = std::make_unique<char[]>(64000);
        FILE __AUTO(__autofile) * fp = fopen(fname.c_str(), "rb");
        if (!fp)
            break;

        do
        {
            if ((rsz = fread(dbuf.get(), 1, 64000, fp)) <= 0)
                break;

            req = { {'D','A','T','A'}, static_cast<uint32_t>(rsz) };
            if (send(client, (const char*)&req, sizeof(req), 0) < static_cast<int32_t>(sizeof(req)))
                break;

            if ((rsz = send(client, dbuf.get(), rsz, 0)) <= 0)
                break;
        }
        while (rsz > 0);

        if (!rsz)
        {
            req = { {'D','O','N','E'}, static_cast<uint32_t>(time(NULL)) };
            if (!f__SessionSendData(client, (const char*)&req, sizeof(req)))
                break;
        }
        else if (rsz < 0)
        {
            if (DriverNet::GetNetError())
                break;
        }
        return true;
    }
    while (0);

    return false;
}

bool ADBDriver::SendFile(std::string const & src, std::string const & dst)
{
    SOCKET l_client = INVALID_SOCKET;

    do
    {
        if ((l_client = m_net.Connect()) == INVALID_SOCKET)
            break;

        if (!f__SessionSendTarget(l_client, m_deviceid))
            break;

        bool ret = f__SessionSendSunc(
                l_client,
                dst,
                DriverConst::ls_cmd_sunc,
                src
            );

        m_net.Close(l_client);
        return ret;
    }
    while (0);

    m_net.Close(l_client);
    return false;
}
