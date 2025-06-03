#pragma once
#include <filesystem>
namespace fs = std::filesystem;
namespace Scripts {
	//Work Scripts
	int StripObjBatchJob(fs::path source, fs::path target, fs::path job_location);
	int ABCtoDatasetBatchJobTimed(fs::path source, fs::path target, fs::path job_location);
	int ABCtoDatasetBatchJob(fs::path source, fs::path target, fs::path job_location);
	int ABCtoDatasetBatch(fs::path source, fs::path target);
	int ABCtoDataset();
	int ABCtoDatasetAE();
	int stripLinesFormOBJ();
	int MeshToSdfSegments();
	//Test Scripts 
	int TestNoisedSDF();
	int ApplySwirlOnMesh();
	int MeshToSdfSegments();
	int StatisticsOnABC();
	int CopyAndRenameYMLandOBJ();
	int SdfToSegmentOnABC();
	int WaveFunctionTest();
	int Output4Python();
	int ParseModelNet();
}