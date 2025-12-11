
#include "../resource.h"

#ifdef ACTION
/*     
            FnTag,      SubFnName,  ClassName,          ActionName,     rcID,            rcName,                Description    
    ----------------------------------------------------------------------------------------------------------------------------------
    Example:
    ==
    1. Action with RC 
    ACTION(L"Dummy",    L"",        L"CDummyAction",    L"Dummy",       IDR_XML_DUMMY,   TOWSTR(IDR_XML_DUMMY),  L"Action with    RC"),
    
    2. Action without RC
    ACTION(L"Dummy",    L"",        L"CDummyAction",    L"Dummy",       0,               NULL,                   L"Action without RC"),

 */
	ACTION(L"GSEditor", L"All",         L"CMakeGS",         L"MakeGolden",   IDR_XML_MAKE,           TOWSTR(IDR_XML_MAKE),            L"Make Golden Sample for All ( Cellular + WCN )"),
    ACTION(L"GSEditor", L"Cellular",    L"CMakeGSCellular", L"MakeGolden",   IDR_XML_MAKE_CELLULAR,  TOWSTR(IDR_XML_MAKE_CELLULAR),   L"Make Golden Sample for Cellular"),
    ACTION(L"GSEditor", L"SC234x",      L"CMakeGSMarlin2",  L"MakeGolden",   IDR_XML_MAKE_MARLIN2,   TOWSTR(IDR_XML_MAKE_MARLIN2),    L"Make Golden Sample for SC234x"),
    ACTION(L"GSEditor", L"UMW265x",     L"CMakeGSMarlin3",  L"MakeGolden",   IDR_XML_MAKE_MARLIN3,   TOWSTR(IDR_XML_MAKE_MARLIN3),    L"Make Golden Sample for UMW265x"),
	ACTION(L"GSEditor", L"Cell_ROC1",   L"CMakeGSCellular", L"MakeGolden",   IDR_XML_MAKE_CELL_ROC1, TOWSTR(IDR_XML_MAKE_CELL_ROC1),  L"Make Golden Sample for Cellular_roc1"),
    ACTION(L"GSCLC",    L"All",         L"CCalcLS",         L"LossCal",      IDR_XML_CALC,           TOWSTR(IDR_XML_CALC),            L"Calculate Loss for All ( Cellular + WCN )"),
    ACTION(L"GSCLC",    L"Cellular",    L"CCalcLSCellular", L"LossCal",      IDR_XML_CALC_CELLULAR,  TOWSTR(IDR_XML_CALC_CELLULAR),   L"Calculate Loss for Cellular"),
    ACTION(L"GSCLC",    L"SC234x",      L"CCalcLSMarlin2",  L"LossCal",      IDR_XML_CALC_MARLIN2,   TOWSTR(IDR_XML_CALC_MARLIN2),    L"Calculate Loss for SC234x"),
    ACTION(L"GSCLC",    L"UMW265x",     L"CCalcLSMarlin3",  L"LossCal",      IDR_XML_CALC_MARLIN3,   TOWSTR(IDR_XML_CALC_MARLIN3),    L"Calculate Loss for UMW265x"),
    
    ACTION(L"GSEditor", L"DMR",         L"CMakeGS",         L"MakeGolden",   IDR_XML_MAKE_DMR,       TOWSTR(IDR_XML_MAKE_DMR),        L"Make Golden Sample"),
	ACTION(L"GSCLC",    L"DMR",         L"CCalcLS",         L"LossCal",      IDR_XML_CALC_DMR,       TOWSTR(IDR_XML_CALC_DMR),        L"Calculate Loss"),

	ACTION(L"FILE",   L"ExportGoldenSample",      L"CExportGoldenSample",         L"ExportGoldenSample",      IDR_XML_EXPORT_GS,       TOWSTR(IDR_XML_EXPORT_GS),        L"Export data to loss file from gold sample"),
	ACTION(L"FILE",   L"ImportGoldenSample",      L"CImportGoldenSample",         L"ImportGoldenSample",      IDR_XML_IMPORT_GS,       TOWSTR(IDR_XML_IMPORT_GS),        L"Import data to gold sample from loss file"),
    ACTION(L"FILE",   L"ExportGoldenSample_V4",   L"CExportGoldenSample_V4",      L"ExportGoldenSample",      IDR_XML_EXPORT_GS,       TOWSTR(IDR_XML_EXPORT_GS),        L"Export data to loss file from gold sample for V4"),
    ACTION(L"FILE",   L"ImportGoldenSample_V4",   L"CImportGoldenSampleV4",       L"ImportGoldenSample",      IDR_XML_IMPORT_GS,       TOWSTR(IDR_XML_IMPORT_GS),        L"Import data to gold sample from loss file V4"),

    ACTION(L"GSEditor", L"N6 Pro", L"CMakeGSCellularV4", L"MakeGolden", IDR_XML_MAKE_CELL_ROC1, TOWSTR(IDR_XML_MAKE_CELL_ROC1), L"Make Golden Sample for N6Pro"),
    ACTION(L"GSCLC", L"N6 Pro", L"CCalcLSCellularV4", L"LossCal", IDR_XML_CALC_CELLULAR, TOWSTR(IDR_XML_CALC_CELLULAR), L"Calculate Loss for Cellular"),

    ACTION(L"GSEditor", L"8520", L"CMakeGSCellularV5", L"MakeGolden", IDR_XML_MAKE_CELL_ROC1, TOWSTR(IDR_XML_MAKE_CELL_ROC1), L"Make Golden Sample for 8520"),
    ACTION(L"GSCLC", L"8520", L"CCalcLSCellularV5", L"LossCal", IDR_XML_CALC_CELLULAR, TOWSTR(IDR_XML_CALC_CELLULAR), L"Calculate Loss for 8520"),

	ACTION(L"GSEditor", L"UMW266x(W6)", L"CMakeGSUmw266x", L"MakeGolden", IDR_XML_MAKE_UMW266X, TOWSTR(IDR_XML_MAKE_UMW266X), L"Make Golden Sample for UMW266x(SongShanW6)"),
	ACTION(L"GSCLC", L"UMW266x(W6)", L"CCalcLSUmw266x", L"LossCal", IDR_XML_CALC_UMW266X, TOWSTR(IDR_XML_CALC_UMW266X), L"Calculate Loss for UMW266x(SongShanW6)"),
    ACTION(L"FILE", L"ExportGoldenSample_V5", L"CExportGoldenSample_V5", L"ExportGoldenSample", IDR_XML_EXPORT_GS, TOWSTR(IDR_XML_EXPORT_GS), L"Export data to loss file from gold sample for V5"),
    ACTION(L"FILE", L"ImportGoldenSample_V5", L"CImportGoldenSampleV5", L"ImportGoldenSample", IDR_XML_IMPORT_GS, TOWSTR(IDR_XML_IMPORT_GS), L"Import data to gold sample from loss file V5"),

#endif 
