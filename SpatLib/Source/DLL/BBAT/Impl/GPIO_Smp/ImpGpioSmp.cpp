#include "StdAfx.h"
#include "ImpGpioSmp.h"
#include <fstream>
#include <iostream>
#include <sstream>

IMPLEMENT_RUNTIME_CLASS(CImpGpioSmp)

CImpGpioSmp::CImpGpioSmp(void)
{
}
CImpGpioSmp::~CImpGpioSmp(void)
{
}

SPRESULT CImpGpioSmp::__PollAction(void)
{
    SPRESULT Res = GpioAction();
    return Res;
}

BOOL CImpGpioSmp::ReadGpioSetFile(LPCTSTR lpszFileName)
{
    ifstream file(lpszFileName);
    if (!file.is_open()) {
        LogFmtStrA(SPLOGLV_ERROR, "Failed to open file %s", lpszFileName);
        return FALSE;
    }

    string line;

    bool errorOccurred = false;

    // 忽略第一行（表头）
    getline(file, line);

    int dataLineCount = 0;
    while (getline(file, line) && dataLineCount < MAXGIPOPAIRCOUNTS) {
        stringstream ss(line);
        string item;
        vector<uint8_t> numbers;
        int columnCount = 0;

        // 只读取前两列
        while (getline(ss, item, ',') && columnCount < 2) {
            if (item.empty()) {
                LogFmtStrA(SPLOGLV_ERROR, "There is empty gpio data");
                errorOccurred = true;
                break;
            }
            try {
                int num = stoi(item);
                if (num < 0 || num > 255) {
                    throw out_of_range("Gpio value is out of uint8_t range");
                }
                numbers.push_back(static_cast<uint8_t>(num));
            }
            catch (exception& e) {
                LogFmtStrA(SPLOGLV_ERROR, "Error: %s in data: %s", e.what(), item);
                errorOccurred = true;
                break;
            }
            columnCount++;
        }

        if (!errorOccurred && numbers.size() == 2) {
            m_vec_GpioPairs.push_back({ numbers[0], numbers[1] });
        }
        dataLineCount++;

        if (errorOccurred) {
            break;
        }
    }
    file.close();

    if (errorOccurred) {
        m_vec_GpioPairs.clear();
        return FALSE;
    }

    if (!errorOccurred) {
        for (const auto& pair : m_vec_GpioPairs) {
            LogFmtStrA(SPLOGLV_INFO, "Write_GPIO = %d; Read_GPIO = %d", pair.first, pair.second);
        }
    }

    return TRUE;
}

BOOL CImpGpioSmp::LoadXMLConfig(void)
{
    m_vecTestItems.clear();
    m_bFailStop = GetConfigValue(L"Param:GPIO:FailStop", 0);
    m_bSetByFile = GetConfigValue(L"Param:GPIO:SetByFile", 0);

    if (m_bSetByFile)
    {
        GetAppPath();
        _tcscpy_s(m_szGpioCsvPath, m_szAppPath);
        _tcscat_s(m_szGpioCsvPath, L"\\Project\\BBAT\\csv\\GPIO_SET.csv");
        m_strGpioCsvPath = GetConfigValue(L"Param:GPIO:GPIO_SetFilePath", m_szGpioCsvPath);
        if (!ReadGpioSetFile(m_strGpioCsvPath.c_str()))
        {
            return FALSE;
        }
        for (int i = 0; i < m_vec_GpioPairs.size(); i++)
        {
            TGroupGpioSmpParam tGroup;
            ZeroMemory(&tGroup, sizeof(tGroup));
            tGroup.byWriteGpioNo = (BYTE)m_vec_GpioPairs[i].first;
            tGroup.byReadGpioNo = (BYTE)m_vec_GpioPairs[i].second;
            m_vecTestItems.push_back(tGroup);
        }
    }
    else
    {
        m_nGroupSize = GetConfigValue(L"Param:GPIO:GroupCount", 0);
        wstring sBase = L"Param:GPIO:";
        for (int i = 0; i < m_nGroupSize; i++)
        {
            TGroupGpioSmpParam tGroup;
            ZeroMemory(&tGroup, sizeof(tGroup));
            wchar_t    szGroup[20] = { 0 };
            swprintf_s(szGroup, L"Group%d:", i + 1);
            wstring sTemp = sBase + szGroup;
            wstring sPath = sTemp + L"Write_GPIO";
            tGroup.byWriteGpioNo = (BYTE)GetConfigValue(sPath.c_str(), 0);
            sPath = sTemp + L"Read_GPIO";
            tGroup.byReadGpioNo = (BYTE)GetConfigValue(sPath.c_str(), 0);
            m_vecTestItems.push_back(tGroup);
        }
    }

    BuildSteps();

    return TRUE;
}

void CImpGpioSmp::BuildSteps()
{
    size_t nVecSize = m_vecTestItems.size();
    for (size_t i = 0; i < nVecSize; i++)
    {
        CGpioSmpBase* Step = nullptr;
        Step = new CGpioSmpBase(m_vecTestItems[i], this);
        if (Step != nullptr)
        {
            m_vecSteps.push_back(Step);
        }
    }
}

SPRESULT CImpGpioSmp::GpioAction()
{
    SPRESULT res = SP_OK;
    size_t nVecSize = m_vecSteps.size();
    for (size_t i = 0; i < nVecSize; i++)
    {
        if (m_bFailStop)
        {
            CHKRESULT(m_vecSteps[i]->Run());
        }
        else
        {
            res |= m_vecSteps[i]->Run();
        }        
    }

    return res;
}

SPRESULT CImpGpioSmp::__FinalAction()
{
    size_t nVecSize = m_vecSteps.size();
    for (size_t i = 0; i < nVecSize; i++)
    {
        if (NULL != m_vecSteps[i])
        {
            delete m_vecSteps[i];
            m_vecSteps[i] = NULL;
        }
    }
    m_vecSteps.clear();
    return SP_OK;
}