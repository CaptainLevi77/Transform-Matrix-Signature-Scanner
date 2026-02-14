#include<iostream>
#include<fstream>
#include<vector>
#include<unordered_set>
#include<queue>
#include<cmath>
#include<string>

/*STRUCTURE*/

struct Candidate
{
	float m[16];
	float score;

	bool operator<(const Candidate& other)const
	{
		return score < other.score;

	}
};


/*HELPER FUNCTION*/

float length3(float x, float y, float z)
{
	return sqrt(x * x + y * y + z * z);
}

float dot3(float ax, float ay, float az, float bx, float by, float bz)
{
	return (ax * bx) + (ay * by) + (az * bz);
}

/*MAIN PROGRAM*/

int main()
{
	/*GET USER INPUT*/

	std::string filename;
	int minimuscore;
	int topK;

	std::cout << "Enter raw data file name: ";
	std::cin >> filename;

	std::cout << "Enter minimu score: ";
	std::cin >> minimuscore;

	std::cout << "Enter TOP K results: ";
	std::cin >> topK;

	/*READ RAW DAT FILE*/

	std::ifstream file(filename, std::ios::binary);
	if (!file)
	{
		std::cout << "File not found\n";
		return 0;
	}

	std::vector<float>rawData;
	float value;

	while (file.read(reinterpret_cast<char*>(&value), sizeof(float)))
	{
		rawData.push_back(value);
	}
	file.close();

	std::cout << "Float read: " << rawData.size() << '\n';

	/*INITIALIZE DATA STRUCTURES*/
	std::priority_queue<Candidate>bestCandidates;
	//std::unordered_set<size_t>visitedpositions;

	/*SCAN THROUGH RAW DATA*/

	for (size_t pos = 0;pos + 15 < rawData.size(); pos++)
	{
		float m[16];
		for (int i = 0; i < 16; i++)
			m[i] = rawData[pos + i];

		float score = 0.0f;

		/* --- Extract vectors --- */

		float rx = m[0], ry = m[1], rz = m[2];
		float ux = m[4], uy = m[5], uz = m[6];
		float fx = m[8], fy = m[9], fz = m[10];


		/* --- Length checks --- */

		if (fabs(length3(rx, ry, rz) - 1.0f) < 0.2f) score++;
		if (fabs(length3(ux, uy, uz) - 1.0f) < 0.2f) score++;
		if (fabs(length3(fx, fy, fz) - 1.0f) < 0.2f) score++;

		/* --- Orthogonality checks --- */

		if (fabs(dot3(rx, ry, rz, ux, uy, uz)) < 0.01f)score++;
		if (fabs(dot3(rx, ry, rz, fx, fy, fz)) < 0.01f)score++;
		if (fabs(dot3(ux, uy, uz, fx, fy, fz)) < 0.01f)score++;

		/* --- Homogeneous matrix check --- */

		if (fabs(m[15] - 1.0f) < 0.01f &&
			fabs(m[3]) < 0.01f &&
			fabs(m[7]) < 0.01f &&
			fabs(m[11]) < 0.01f)
		{
			score++;
		}

		/*Translation sanity check*/
		if (std::isfinite(m[12]) && std::isfinite(m[13]) && std::isfinite(m[14]))
		{
			score++;
		}


		/*Save Candidates if score is high enough*/

		if (score >= minimuscore)
		{
			Candidate c;
			for (int i = 0;i < 16;i++)
				c.m[i] = m[i];

			c.score = score;
			bestCandidates.push(c);

			if ((int)bestCandidates.size() > topK)
				bestCandidates.pop();

		}
	}

	/* -------- OUTPUT RESULTS -------- */

	std::cout << "Top camera matrix candidate: ";

	for (int k = 0;k < topK && !bestCandidates.empty();k++)
	{
		Candidate c = bestCandidates.top();
		bestCandidates.pop();

		std::cout << "\nScore: " << c.score << "Matrix: \n";


		for (int i = 0;i < 16;i++)
		{
			std::cout << c.m[i] << " ";

			if ((i + 1) % 4 == 0)
				std::cout << "\n";
		}

	}

	return 0;
}

