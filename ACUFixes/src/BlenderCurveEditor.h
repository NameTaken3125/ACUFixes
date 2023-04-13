#pragma once

#include <vector>
#include <optional>

// Control points and values are currently expected to be within [0.0, 1.0].
class BlenderCurveEditor
{
public:
	BlenderCurveEditor(const std::vector<ImVec2>& controlPoints = { {0, 0}, {1, 1} });
	// Returns true if modified.
	bool Draw(ImVec2 editorSize = { 350, 350 });
	float Evaluate(float x);
	std::vector<float> Samples(size_t numSamples);
	void Samples(size_t numSamples, std::vector<float>& resultOut);

	const std::vector<ImVec2>& GetControlPoints();

private:
	std::vector<ImVec2> m_controlPoints;
	std::optional<size_t> m_selectedPoint;

public:
	ImVec2 m_VisibleRangeLeftBottom = { 0, 0 };
	ImVec2 m_VisibleRangeRightTop = { 1, 1 };
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

	CurveEvaluationHelper(const std::vector<ImVec2>& controlPoints, float extrapolationXMin = 0.0f, float extrapolationXMax = 1.0f);
	// After successfully initialized.
	float Evaluate(float x);
};
