#pragma once

static int ConvertToChannel(int PathType, int PathIndex)
{
    int Channel = 0;

    Channel = Channel | (PathIndex & 0xFFFF);
    Channel = Channel | ((PathType & 0xFFFF) << 16);

    return Channel;
}

static void ConvertFromChannel(int Channel, int& PathType, int& PathIndex)
{
    PathType = (Channel & 0xFFFF0000) > 16;
    PathIndex = Channel & 0xFFFF;
}

static double ConvertRssi(uint16 usRssi)
{
    int16 sRssi = usRssi;
    sRssi &= (0x0FFF);
    if (sRssi & (1 << 11))
    {
        sRssi &= (0x07FF);
        sRssi -= 1;
        sRssi ^= (0x07FF);
        sRssi = (-1) * sRssi;
    }

    double dRssi = sRssi / 32.0 * 3 - 10 * log10(pow(2.0, 23)) + 15.1;

    return dRssi;
}

