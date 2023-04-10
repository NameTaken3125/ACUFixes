#pragma once

// Control points and values are currently expected to be within [0.0, 1.0].
class BlenderCurveEditor
{
public:
	BlenderCurveEditor();
	BlenderCurveEditor(const std::vector<ImVec2>& controlPoints);
	bool Draw();
	float Evaluate(float x);
	std::vector<float> Samples(size_t numSamples);
	void Samples(size_t numSamples, std::vector<float>& resultOut);

	void GetControlPointsCopy(std::vector<ImVec2>& resultOut);

private:
	std::vector<ImVec2> m_controlPoints;
	std::optional<size_t> m_selectedPoint;
};

// Adapted from Blender's `CurveMap`
struct CurveEvaluationHelper
{
	/** Quick multiply value for reading table. */
	float range;
	/** The x-axis range for the table. */
	float mintable, maxtable;
	/** For extrapolated curves, the direction vector. */
	ImVec2 ext_in, ext_out;
	/** Display and evaluate table. */
	std::vector<ImVec2> equidistantSamples;

	CurveEvaluationHelper(const std::vector<ImVec2>& controlPoints);
	// After successfully initialized.
	float Evaluate(float x);
};
