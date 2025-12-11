#include "StdAfx.h"
#include "NalClc.h"
#include "assert.h"
#include "LteUtility.h"
#include <algorithm>

CNalClc::CNalClc( LPCWSTR lpName, CFuncCenter* pFuncCenter ) : CFuncBase(lpName, pFuncCenter)
{
    m_plossVal = NULL;
	m_pNvHelper = NULL;
}

CNalClc::~CNalClc(void)
{

}

SPRESULT CNalClc::PreInit()
{
    CHKRESULT(__super::PreInit());

	m_pNvHelper = (CNvHelper*)GetFunc(FUNC_INVHelper);
    m_plossVal = &((CImpBase*)m_pSpatBase)->m_lossVal;

    return SP_OK;
}

SPRESULT CNalClc::SetResult()
{
    std::vector<RF_CABLE_LOSS_POINT_EX> arrLoss;
    arrLoss.clear();

    for (int j = 0; j < MAX_LTE_ANT; j++)
    {
        if (m_arrConfig[j].size() == 0)
        {
            continue;
        }
        for (uint32 i = 0; i < m_arrConfig[j].size(); i++)
        {
            Config* pConfig = &m_arrConfig[j][i];
            Result* pResult = &m_arrResult[j][i];

            if (pConfig->byPath == 0)
            {
                continue;
            }

            RF_IO_E TRX = RF_IO_TX;
            if (pConfig->byPath == RX)
            {
                TRX = RF_IO_RX;
            }

            std::vector<RF_CABLE_LOSS_POINT_EX>::iterator itor;
            itor = std::find_if(arrLoss.begin(), arrLoss.end(), 
                vector_finder(pConfig->byBand, pConfig->usChannel, pConfig->dFreq, TRX));
            if (itor == arrLoss.end())
            {
                RF_CABLE_LOSS_POINT_EX item;
                memset(&item, 0, sizeof(item));
                item.nBand = pConfig->byBand;
                item.dFreq[TRX] = pConfig->dFreq;
                item.uArfcn[TRX] = pConfig->usChannel;
                item.dLoss[pResult->usAnt][TRX] = pResult->dLoss;
                //item.dLoss[pResult->usAnt][RF_IO_RX] = pResult->dLoss;
                arrLoss.push_back(item);
            }
            else
            {
                (*itor).nBand = pConfig->byBand;
                (*itor).dFreq[TRX] = pConfig->dFreq;
                (*itor).uArfcn[TRX] = pConfig->usChannel;
                (*itor).dLoss[pResult->usAnt][TRX] = pResult->dLoss;
                //(*itor).dLoss[pResult->usAnt][RF_IO_RX] = pResult->dLoss;
            }
        }
    }

    //std::sort(arrLoss.begin(), arrLoss.end(), cmp());

    memset(&m_plossVal->lteLoss, 0, sizeof(m_plossVal->lteLoss));
    m_plossVal->lteLoss.nCount = arrLoss.size();
    for (uint32 i = 0; i < arrLoss.size(); i++)
    {
        m_plossVal->lteLoss.arrPoint[i].nBand = arrLoss[i].nBand;
        m_plossVal->lteLoss.arrPoint[i].dFreq[RF_IO_TX] = arrLoss[i].dFreq[RF_IO_TX];
        m_plossVal->lteLoss.arrPoint[i].dFreq[RF_IO_RX] = arrLoss[i].dFreq[RF_IO_RX];
        m_plossVal->lteLoss.arrPoint[i].uArfcn[RF_IO_TX] = arrLoss[i].uArfcn[RF_IO_TX];
        m_plossVal->lteLoss.arrPoint[i].uArfcn[RF_IO_RX] = arrLoss[i].uArfcn[RF_IO_RX];
        for (int j = 0; j < MAX_RF_ANT; j++)
        {
            m_plossVal->lteLoss.arrPoint[i].dLoss[j][RF_IO_TX] = arrLoss[i].dLoss[j][RF_IO_TX];
            m_plossVal->lteLoss.arrPoint[i].dLoss[j][RF_IO_RX] = arrLoss[i].dLoss[j][RF_IO_RX];
        }
    }

	/*for(uint32 m = 0; m < arrLoss.size(); m++)
	{
		for (int j = 0; j < MAX_RF_ANT; j++)
		{
			if(IS_EQUAL(m_plossVal->lteLoss.arrPoint[m].dLoss[j][RF_IO_TX],0))
			{
				bool bDone = false;
				if( 0 == m)
				{
					for( uint32 iInsteadIndex = 1;iInsteadIndex < arrLoss.size();iInsteadIndex++)
					{
						if(m_plossVal->lteLoss.arrPoint[iInsteadIndex].dLoss[j][RF_IO_TX] > 0)
						{
							m_plossVal->lteLoss.arrPoint[m].dLoss[j][RF_IO_TX] = m_plossVal->lteLoss.arrPoint[iInsteadIndex].dLoss[j][RF_IO_TX];
							m_plossVal->lteLoss.arrPoint[m].dLoss[j][RF_IO_RX] = m_plossVal->lteLoss.arrPoint[iInsteadIndex].dLoss[j][RF_IO_RX];
							bDone = true;
                            break;
						}
					}
				}
				else
				{
					for( uint32 iInsteadIndex = 0;iInsteadIndex < m;iInsteadIndex++)
					{
						if(m_plossVal->lteLoss.arrPoint[iInsteadIndex].dLoss[j][RF_IO_TX] > 0)
						{
							m_plossVal->lteLoss.arrPoint[m].dLoss[j][RF_IO_TX] = m_plossVal->lteLoss.arrPoint[iInsteadIndex].dLoss[j][RF_IO_TX];
							m_plossVal->lteLoss.arrPoint[m].dLoss[j][RF_IO_RX] = m_plossVal->lteLoss.arrPoint[iInsteadIndex].dLoss[j][RF_IO_RX];
							bDone = true;
						}
					}

					if(!bDone)
					{
						for( uint32 iInsteadIndex = m+1;iInsteadIndex < arrLoss.size();iInsteadIndex++)
						{
							if(m_plossVal->lteLoss.arrPoint[iInsteadIndex].dLoss[j][RF_IO_TX] > 0)
							{
								m_plossVal->lteLoss.arrPoint[m].dLoss[j][RF_IO_TX] = m_plossVal->lteLoss.arrPoint[iInsteadIndex].dLoss[j][RF_IO_TX];
								m_plossVal->lteLoss.arrPoint[m].dLoss[j][RF_IO_RX] = m_plossVal->lteLoss.arrPoint[iInsteadIndex].dLoss[j][RF_IO_RX];;
								bDone = true;
                                break;
							}
						}
					}
				}

			}
		}
	}*/

    return SP_OK;
}

SPRESULT CNalClc::ClearResult()
{
    ClearResult(LTE_ANT_MAIN);
    ClearResult(LTE_ANT_DIV );

    return SP_OK;
}

SPRESULT CNalClc::ClearResult( LTE_ANT_E Ant )
{
    if (m_arrResult[Ant].size() != 0)
    {
        memset(&m_arrResult[Ant][0], 0, sizeof(Result) * m_arrResult[Ant].size());
    }

    return SP_OK;
}

SPRESULT CNalClc::InitResult()
{
    InitResult(LTE_ANT_MAIN);
    InitResult(LTE_ANT_DIV );

    return SP_OK;
}

SPRESULT CNalClc::InitResult( LTE_ANT_E Ant )
{
    m_arrResult[Ant].resize(m_arrConfig[Ant].size());

    return SP_OK;
}

void CNalClc::Deserialization( std::vector<uint8>* parrData )
{
    uint16 usMainLength = *(uint16*)&(*parrData)[0];
    uint16 usDivLength = *(uint16*)&(*parrData)[2];

    if (parrData->size()!= (uint32)(4 + usMainLength + usDivLength))
    {
        assert(0);
    }

    m_arrConfig[LTE_ANT_MAIN].clear();
    m_arrConfig[LTE_ANT_DIV].clear();

    if (usMainLength != 0)
    {
        Deserialization(&(*parrData)[4], usMainLength, LTE_ANT_MAIN);
    }
    if (usDivLength != 0)
    {
        Deserialization(&(*parrData)[4 + usMainLength], usDivLength, LTE_ANT_DIV);
    }

	if(NULL != m_pNvHelper->m_pNvV3)
	{
		m_pNvHelper->m_pNvV3->m_nClcBand.clear();
		for(uint32 i = 0;i < m_arrConfig[LTE_ANT_MAIN].size();i++)
		{
			int nBand = m_arrConfig[LTE_ANT_MAIN][i].byBand;
			vector<int>::iterator result = find( m_pNvHelper->m_pNvV3->m_nClcBand.begin( ), m_pNvHelper->m_pNvV3->m_nClcBand.end( ), nBand ); //²éÕÒ3
			if ( result == m_pNvHelper->m_pNvV3->m_nClcBand.end( ) ) 
			{
				m_pNvHelper->m_pNvV3->m_nClcBand.push_back(nBand);
			}
		}
	}
}

void CNalClc::Deserialization( uint8* pData, uint16 usLength, LTE_ANT_E Ant )
{
    uint16 usIndex = 0;
    BOOL bEnd = FALSE;
    while (!bEnd)
    {
        if ((usLength - usIndex) < 7)
        {
            assert(0);
        }

        Config Item;
		Item.dFreq = (double)(*((uint16*)pData) / 10);
		pData += 2;
		Item.usChannel = *((uint32*)pData);
		pData += 4;
        Item.byIndicator = (uint8)CLteUtility::m_BandInfo[*pData].nIndicator;
		Item.byBand = *((uint8*)pData);
        pData++;
        usIndex += 7;

        if (*pData == TX)
        {
            pData++;
            usIndex += 1;
            if ((usLength - usIndex) < 4)
            {
                assert(0);
                LogFmtStrA(SPLOGLV_ERROR, "Tx Loss data Error!");
            }

            Item.byPath = TX;
            Item.Tx.usWord = *((uint16*)pData);
            pData += 2;
            Item.Tx.dPower = (double)(*((int16*)pData) / 100.0);
            pData += 2;
            m_arrConfig[Ant].push_back(Item);
            usIndex += 4;
        }
        else
        {
            pData++;
            usIndex += 1;
            if ((usLength - usIndex) < 5)
            {
                LogFmtStrA(SPLOGLV_ERROR, "Rx Loss data Error!");
                assert(0);
            }

            Item.byPath = RX;
            Item.Rx.dRssi = (double)(*((int16*)pData) / 100.0);
            pData += 2;
            Item.Rx.dCellPower = (double)(*((int16*)pData) / 100.0);
            pData += 2;
            Item.Rx.byIndex = *((uint8*)pData);
            pData ++;
            m_arrConfig[Ant].push_back(Item);
            usIndex += 5;
        }

        if (usIndex > usLength)
        {
            LogFmtStrA(SPLOGLV_ERROR, "Loss data Error!");
            assert(0);
        }

        if (usIndex == usLength)
        {
            bEnd = TRUE;
        }
    }
}
