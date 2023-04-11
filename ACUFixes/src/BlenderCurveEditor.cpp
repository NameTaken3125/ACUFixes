
#include <algorithm>
#include "imgui/imgui.h"

#include "BlenderCurveEditor.h"

#include "vmath/vmath.h"



#define IMGUI_STYLE_EDIT_SHOW_CURVE_EDITOR_COLOR_PICKERS false



#ifndef IMGUI_DEFINE_MATH_OPERATORS
static inline ImVec2 operator*(const ImVec2& lhs, const float rhs) { return ImVec2(lhs.x * rhs, lhs.y * rhs); }
static inline ImVec2 operator/(const ImVec2& lhs, const float rhs) { return ImVec2(lhs.x / rhs, lhs.y / rhs); }
static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }
static inline ImVec2 operator*(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x * rhs.x, lhs.y * rhs.y); }
static inline ImVec2 operator/(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x / rhs.x, lhs.y / rhs.y); }
static inline ImVec2& operator*=(ImVec2& lhs, const float rhs) { lhs.x *= rhs; lhs.y *= rhs; return lhs; }
static inline ImVec2& operator/=(ImVec2& lhs, const float rhs) { lhs.x /= rhs; lhs.y /= rhs; return lhs; }
static inline ImVec2& operator+=(ImVec2& lhs, const ImVec2& rhs) { lhs.x += rhs.x; lhs.y += rhs.y; return lhs; }
static inline ImVec2& operator-=(ImVec2& lhs, const ImVec2& rhs) { lhs.x -= rhs.x; lhs.y -= rhs.y; return lhs; }
static inline ImVec2& operator*=(ImVec2& lhs, const ImVec2& rhs) { lhs.x *= rhs.x; lhs.y *= rhs.y; return lhs; }
static inline ImVec2& operator/=(ImVec2& lhs, const ImVec2& rhs) { lhs.x /= rhs.x; lhs.y /= rhs.y; return lhs; }
static inline ImVec4 operator+(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w); }
static inline ImVec4 operator-(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w); }
static inline ImVec4 operator*(const ImVec4& lhs, const ImVec4& rhs) { return ImVec4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w); }
#endif

ImVec2 Clamp_01(ImVec2 pt) {
	if (pt.x < 0) { pt.x = 0; }
	else if (pt.x > 1) { pt.x = 1; }
	if (pt.y < 0) { pt.y = 0; }
	else if (pt.y > 1) { pt.y = 1; }
	return pt;
};
void SortPointsByX(std::vector<ImVec2>& points)
{
	auto b = std::begin(points);
	auto e = std::end(points);
	std::sort(b, e, [](const ImVec2& a, const ImVec2& b) { return a.x < b.x; });
};


struct BezTriple
{
	ImVec2 left;
	ImVec2 center;
	ImVec2 right;
};

// From `Blender Source Code -> colortools.c -> curvemap_make_table()`
namespace ThanksBlender
{
	void madd_v2_v2v2fl(ImVec2& r, const ImVec2& a, const ImVec2& b, float f)
	{
		r.x = a.x + b.x * f;
		r.y = a.y + b.y * f;
	}
	/**
	 * reduced copy of #calchandleNurb_intern code in curve.c
	 */
	static void calchandle_curvemap(BezTriple* bezt, const BezTriple* prev, const BezTriple* next)
	{
		/* defines to avoid confusion */
#define p2_h1 (((BezTriple*)(p2 - 1))->left)
#define p2_h2 (((BezTriple*)(p2 - 1))->right)

		const ImVec2* p1, * p3;
		ImVec2* p2 = &bezt->center;
		ImVec2 pt;
		float len, len_a, len_b;


		if (prev == NULL) {
			p3 = &next->center;
			pt.x = 2.0f * p2->x - p3->x;
			pt.y = 2.0f * p2->y - p3->y;
			p1 = &pt;
		}
		else {
			p1 = &prev->center;
		}

		if (next == NULL) {
			p1 = &prev->center;
			pt.x = 2.0f * p2->x - p1->x;
			pt.y = 2.0f * p2->y - p1->y;
			p3 = &pt;
		}
		else {
			p3 = &next->center;
		}

		ImVec2 dvec_a = *p2 - *p1;
		ImVec2 dvec_b = *p3 - *p2;

		len_a = ((Vector2f&)dvec_a).length();
		len_b = ((Vector2f&)dvec_b).length();

		if (len_a == 0.0f) {
			len_a = 1.0f;
		}
		if (len_b == 0.0f) {
			len_b = 1.0f;
		}

		//#define ELEM(expr, value1, value2) (((expr) == (value1)) || (expr) == (value2)))
			//if (ELEM(bezt->h1, HD_AUTO, HD_AUTO_ANIM) || ELEM(bezt->h2, HD_AUTO, HD_AUTO_ANIM)) { /* auto */
		{
			ImVec2 tvec;
			tvec.x = dvec_b.x / len_b + dvec_a.x / len_a;
			tvec.y = dvec_b.y / len_b + dvec_a.y / len_a;

			len = ((Vector2f&)tvec).length() * 2.5614f;
			if (len != 0.0f) {

				//if (ELEM(bezt->h1, HD_AUTO, HD_AUTO_ANIM)) {
				{
					len_a /= len;
					madd_v2_v2v2fl(p2_h1, *p2, tvec, -len_a);


					//if ((bezt->h1 == HD_AUTO_ANIM) && next && prev) { /* keep horizontal if extrema */
					//	const float ydiff1 = prev->vec[1][1] - bezt->vec[1][1];
					//	const float ydiff2 = next->vec[1][1] - bezt->vec[1][1];
					//	if ((ydiff1 <= 0.0f && ydiff2 <= 0.0f) || (ydiff1 >= 0.0f && ydiff2 >= 0.0f)) {
					//		bezt->vec[0][1] = bezt->vec[1][1];
					//	}
					//	else { /* handles should not be beyond y coord of two others */
					//		if (ydiff1 <= 0.0f) {
					//			if (prev->vec[1][1] > bezt->vec[0][1]) {
					//				bezt->vec[0][1] = prev->vec[1][1];
					//			}
					//		}
					//		else {
					//			if (prev->vec[1][1] < bezt->vec[0][1]) {
					//				bezt->vec[0][1] = prev->vec[1][1];
					//			}
					//		}
					//	}
					//}
				}
				//if (ELEM(bezt->h2, HD_AUTO, HD_AUTO_ANIM)) {
				{
					len_b /= len;
					madd_v2_v2v2fl(p2_h2, *p2, tvec, len_b);

					//if ((bezt->h2 == HD_AUTO_ANIM) && next && prev) { /* keep horizontal if extrema */
					//	const float ydiff1 = prev->vec[1][1] - bezt->vec[1][1];
					//	const float ydiff2 = next->vec[1][1] - bezt->vec[1][1];
					//	if ((ydiff1 <= 0.0f && ydiff2 <= 0.0f) || (ydiff1 >= 0.0f && ydiff2 >= 0.0f)) {
					//		bezt->vec[2][1] = bezt->vec[1][1];
					//	}
					//	else { /* handles should not be beyond y coord of two others */
					//		if (ydiff1 <= 0.0f) {
					//			if (next->vec[1][1] < bezt->vec[2][1]) {
					//				bezt->vec[2][1] = next->vec[1][1];
					//			}
					//		}
					//		else {
					//			if (next->vec[1][1] > bezt->vec[2][1]) {
					//				bezt->vec[2][1] = next->vec[1][1];
					//			}
					//		}
					//	}
					//}
				}
			}
		}

		//if (bezt->h1 == HD_VECT) { /* vector */
		//	madd_v2_v2v2fl(p2_h1, p2, dvec_a, -1.0f / 3.0f);
		//}
		//if (bezt->h2 == HD_VECT) {
		//	madd_v2_v2v2fl(p2_h2, p2, dvec_b, 1.0f / 3.0f);
		//}

#undef p2_h1
#undef p2_h2
	}
	/* This function is almost the same as BKE_fcurve_correct_bezpart(), but doesn't allow as large a
	 * tangent. */
	void BKE_curve_correct_bezpart(const ImVec2& v1, ImVec2& v2, ImVec2& v3, const ImVec2& v4)
	{
		ImVec2 h1, h2;
		float len1, len2, len, fac;

		/* Calculate handle deltas. */
		h1.x = v1.x - v2.x;
		h1.y = v1.y - v2.y;

		h2.x = v4.x - v3.x;
		h2.y = v4.y - v3.y;

		/* Calculate distances:
		 * - len  = span of time between keyframes
		 * - len1 = length of handle of start key
		 * - len2 = length of handle of end key
		 */
		len = v4.x - v1.x;
		len1 = fabsf(h1.x);
		len2 = fabsf(h2.x);

		/* If the handles have no length, no need to do any corrections. */
		if ((len1 + len2) == 0.0f) {
			return;
		}

		/* the two handles cross over each other, so force them
		 * apart using the proportion they overlap
		 */
		if ((len1 + len2) > len) {
			fac = len / (len1 + len2);

			v2.x = (v1.x - fac * h1.x);
			v2.y = (v1.y - fac * h1.y);

			v3.x = (v4.x - fac * h2.x);
			v3.y = (v4.y - fac * h2.y);
		}
	}
#define POINTER_OFFSET(v, ofs) ((void *)((char *)(v) + (ofs)))
	/* forward differencing method for bezier curve */
	void BKE_curve_forward_diff_bezier(
		float q0, float q1, float q2, float q3, float* p, int it, int stride)
	{
		float rt0, rt1, rt2, rt3, f;
		int a;

		f = (float)it;
		rt0 = q0;
		rt1 = 3.0f * (q1 - q0) / f;
		f *= f;
		rt2 = 3.0f * (q0 - 2.0f * q1 + q2) / f;
		f *= it;
		rt3 = (q3 - q0 + 3.0f * (q1 - q2)) / f;

		q0 = rt0;
		q1 = rt1 + rt2 + rt3;
		q2 = 2 * rt2 + 6 * rt3;
		q3 = 6 * rt3;

		for (a = 0; a <= it; a++) {
			*p = q0;
			p = (float*)POINTER_OFFSET(p, stride);
			q0 += q1;
			q1 += q2;
			q2 += q3;
		}
	}
#undef POINTER_OFFSET


	/* in X, out Y.
	 * X is presumed to be outside first or last */
	static float curvemap_calc_extend(
		float x,
		const ImVec2& first,
		const ImVec2& last,
		const ImVec2& ext_in,
		const ImVec2& ext_out
	)
	{
		if (x <= first.x)
		{
			if (ext_in.x == 0.0f) {
				return first.y + ext_in.y * 10000.0f;
			}
			return first.y + ext_in.y * (x - first.x) / ext_in.x;
		}
		if (x >= last.x)
		{
			if (ext_out.x == 0.0f) {
				return last.y - ext_out.y * 10000.0f;
			}
			return last.y + ext_out.y * (x - last.x) / ext_out.x;
		}
		return 0.0f;
	}

	enum { CM_RESOL = 32 };
	enum { CM_TABLE = 256 };
	// "Quantum" of X.
	const float CM_TABLEDIV = 1.0f / 256.0f;
	/**
	 * Almost-equal for IEEE floats, using absolute difference method.
	 *
	 * \param max_diff: the maximum absolute difference.
	 */
	int compare_ff(float a, float b, const float max_diff)
	{
		return fabsf(a - b) <= max_diff;
	}
	// Calculate bezier handles such that the resulting curve goes through the control points.
	std::vector<BezTriple> CalculateAutomaticBezierHandles(const std::vector<ImVec2>& controlPoints)
	{
		std::vector<BezTriple> pointsWithAutoHandles;
		pointsWithAutoHandles.reserve(controlPoints.size());
		for (ImVec2 pt : controlPoints)
		{
			pointsWithAutoHandles.push_back({ ImVec2(), pt, ImVec2() });
		}
		// Calculate auto handles.
		const BezTriple* bezt_prev = nullptr;
		for (size_t a = 0; a < pointsWithAutoHandles.size(); a++) {
			const BezTriple* bezt_next = (a != pointsWithAutoHandles.size() - 1) ? &pointsWithAutoHandles[a + 1] : NULL;
			ThanksBlender::calchandle_curvemap(&pointsWithAutoHandles[a], bezt_prev, bezt_next);
			bezt_prev = &pointsWithAutoHandles[a];
		}

		/* first and last handle need correction, instead of pointing to center of next/prev,
		 * we let it point to the closest handle */
		if (controlPoints.size() > 2) {
			float hlen, nlen;
			ImVec2 vec;

			//if (bezt[0].h2 == HD_AUTO) {
			{

				hlen = ((Vector2f&)(pointsWithAutoHandles[0].center - pointsWithAutoHandles[0].right)).length(); /* original handle length */
				/* clip handle point */
				vec = pointsWithAutoHandles[1].left;
				if (vec.x < pointsWithAutoHandles[0].center.x) {
					vec.x = pointsWithAutoHandles[0].center.x;
				}

				vec -= pointsWithAutoHandles[0].center;
				nlen = ((Vector2f&)vec).length();
				if (nlen > FLT_EPSILON) {
					vec *= hlen / nlen;
					pointsWithAutoHandles[0].right = vec + pointsWithAutoHandles[0].center;
					pointsWithAutoHandles[0].left = pointsWithAutoHandles[0].center - vec;
				}
			}
			size_t a = pointsWithAutoHandles.size() - 1;
			//if (bezt[a].h2 == HD_AUTO) {
			{

				hlen = ((Vector2f&)(pointsWithAutoHandles[a].center - pointsWithAutoHandles[a].left)).length(); /* original handle length */
				/* clip handle point */
				vec = pointsWithAutoHandles[a - 1].right;
				if (vec.x > pointsWithAutoHandles[a].center.x) {
					vec.x = pointsWithAutoHandles[a].center.x;
				}

				vec -= pointsWithAutoHandles[a].center;
				nlen = ((Vector2f&)vec).length();
				if (nlen > FLT_EPSILON) {
					vec *= hlen / nlen;
					pointsWithAutoHandles[a].left = vec + pointsWithAutoHandles[a].center;
					pointsWithAutoHandles[a].right = pointsWithAutoHandles[a].center - vec;
				}
			}
		}
		return pointsWithAutoHandles;
	}
	// Calculates an equal number of samples for each curve segment
	// (the samples are therefore NOT uniformly distributed on X axis).
	std::vector<ImVec2> CalculateSmoothSamples(std::vector<BezTriple>& pointsWithAutoHandles)
	{
		std::vector<ImVec2> smoothSamples;
		size_t totpoint = (pointsWithAutoHandles.size() - 1) * CM_RESOL;
		smoothSamples.resize(totpoint);

		for (size_t a = 0; a < pointsWithAutoHandles.size() - 1; a++) {
			ImVec2* point = &smoothSamples[CM_RESOL * a];
			ThanksBlender::BKE_curve_correct_bezpart(
				pointsWithAutoHandles[a].center,
				pointsWithAutoHandles[a].right,
				pointsWithAutoHandles[a + 1].left,
				pointsWithAutoHandles[a + 1].center);
			ThanksBlender::BKE_curve_forward_diff_bezier(
				pointsWithAutoHandles[a].center.x,
				pointsWithAutoHandles[a].right.x,
				pointsWithAutoHandles[a + 1].left.x,
				pointsWithAutoHandles[a + 1].center.x,
				&point->x,
				CM_RESOL - 1,
				sizeof(ImVec2));
			ThanksBlender::BKE_curve_forward_diff_bezier(
				pointsWithAutoHandles[a].center.y,
				pointsWithAutoHandles[a].right.y,
				pointsWithAutoHandles[a + 1].left.y,
				pointsWithAutoHandles[a + 1].center.y,
				&point->y,
				CM_RESOL - 1,
				sizeof(ImVec2));
		}
		return smoothSamples;
	}
	static void CalculateExtrapolationVectors(std::vector<BezTriple>& bezt, ImVec2& ext_in, ImVec2& ext_out)
	{
		/* store first and last handle for extrapolation, unit length */
		ext_in.x = bezt[0].left.x - bezt[0].center.x;
		ext_in.y = bezt[0].left.y - bezt[0].center.y;
		float ext_in_range = sqrtf(ext_in.x * ext_in.x +
			ext_in.y * ext_in.y);
		ext_in.x /= ext_in_range;
		ext_in.y /= ext_in_range;

		size_t out_a = bezt.size() - 1;
		ext_out.x = bezt[out_a].center.x - bezt[out_a].right.x;
		ext_out.y = bezt[out_a].center.y - bezt[out_a].right.y;
		float ext_out_range = sqrtf(ext_out.x * ext_out.x +
			ext_out.y * ext_out.y);
		ext_out.x /= ext_out_range;
		ext_out.y /= ext_out_range;
	}
	void CalculateEquidistantSamples(std::vector<ImVec2>& smoothSamples, CurveEvaluationHelper& cumaInOut)
	{
		// "Quantum" of X, adjusted for total X axis length.
		float quantum_x = CM_TABLEDIV * (cumaInOut.maxtable - cumaInOut.mintable);
		cumaInOut.range = 1.0f / quantum_x;
		/* now make a table with CM_TABLE equal x distances */
		ImVec2* firstpoint = &smoothSamples[0];
		ImVec2* lastpoint = firstpoint + (smoothSamples.size() - 1);
		ImVec2* point = firstpoint;

		// cmp
		cumaInOut.equidistantSamples.resize(CM_TABLE + 1);

		for (int a = 0; a <= CM_TABLE; a++) {
			float cur_x = cumaInOut.mintable + quantum_x * (float)a;
			cumaInOut.equidistantSamples[a].x = cur_x;

			/* Get the first point with x coordinate larger than cur_x. */
			while (cur_x >= point->x && point != lastpoint) {
				point++;
			}

			/* Check if we are on or outside the start or end point. */
			if (point == firstpoint || (point == lastpoint && cur_x >= point->x)) {
				if (compare_ff(cur_x, point->x, 1e-6f)) {
					/* When on the point exactly, use the value directly to avoid precision
					 * issues with extrapolation of extreme slopes. */
					cumaInOut.equidistantSamples[a].y = point->y;
				}
				else {
					/* Extrapolate values that lie outside the start and end point. */
					cumaInOut.equidistantSamples[a].y = curvemap_calc_extend(cur_x, *firstpoint, *lastpoint, cumaInOut.ext_in, cumaInOut.ext_out);
				}
			}
			else {
				float fac1 = point->x - point[-1].x;
				float fac2 = point->x - cur_x;
				if (fac1 > FLT_EPSILON) {
					fac1 = fac2 / fac1;
				}
				else {
					fac1 = 0.0f;
				}
				cumaInOut.equidistantSamples[a].y = fac1 * point[-1].y + (1.0f - fac1) * point->y;
			}
		}
	}
	/* only creates a table for a single channel in CurveMapping */
	static void curvemap_make_table(std::vector<BezTriple>& bezt, CurveEvaluationHelper& cumaInOut)
	{
		std::vector<ImVec2> smoothSamples = CalculateSmoothSamples(bezt);
		CalculateExtrapolationVectors(bezt, cumaInOut.ext_in, cumaInOut.ext_out);
		CalculateEquidistantSamples(smoothSamples, cumaInOut);
	}
	/* table should be verified */
	float BKE_curvemap_evaluateF(CurveEvaluationHelper& cuma, float value)
	{
		/* index in table */
		float fi = (value - cuma.mintable) * cuma.range;
		int i = (int)fi;

		/* fi is table float index and should check against table range i.e. [0.0 CM_TABLE] */
		if (fi < 0.0f || fi > CM_TABLE) {
			return curvemap_calc_extend(value, cuma.equidistantSamples[0], cuma.equidistantSamples[CM_TABLE], cuma.ext_in, cuma.ext_out);
		}

		if (i < 0) {
			return cuma.equidistantSamples[0].y;
		}
		if (i >= CM_TABLE) {
			return cuma.equidistantSamples[CM_TABLE].y;
		}

		fi = fi - (float)i;
		return (1.0f - fi) * cuma.equidistantSamples[i].y + fi * cuma.equidistantSamples[(size_t)i + 1].y;
	}
}
CurveEvaluationHelper::CurveEvaluationHelper(const std::vector<ImVec2>& controlPoints, float extrapolationXMin, float extrapolationXMax)
	: mintable(extrapolationXMin), maxtable(extrapolationXMax)
	, range(0)
{
	for (const ImVec2& pt : controlPoints)
	{
		if (mintable > pt.x)
			mintable = pt.x;
		if (maxtable < pt.x)
			maxtable = pt.x;
	}
	std::vector<BezTriple> withBeziers = ThanksBlender::CalculateAutomaticBezierHandles(controlPoints);
	ThanksBlender::curvemap_make_table(withBeziers, *this);
}
float CurveEvaluationHelper::Evaluate(float x)
{
	return ThanksBlender::BKE_curvemap_evaluateF(*this, x);
}

// Prefer to create a `CurveEvaluationHelper` instance if you need many samples.
float EvaluateCurve(const std::vector<ImVec2>& controlPoints, float value)
{
	CurveEvaluationHelper cuma{ controlPoints };
	return ThanksBlender::BKE_curvemap_evaluateF(cuma, value);
}
void EvaluateCurveSamples(const std::vector<ImVec2>& controlPoints, size_t numSamples, std::vector<float>& resultOut)
{
	CurveEvaluationHelper cuma{ controlPoints };

	resultOut.resize(numSamples);
	float xstep = (cuma.maxtable - cuma.mintable) / (numSamples - 1);
	float x = cuma.mintable;
	for (size_t i = 0; i < numSamples; i++, x += xstep)
	{
		resultOut[i] = cuma.Evaluate(x);
	}
}
BlenderCurveEditor::BlenderCurveEditor(const std::vector<ImVec2>& controlPoints)
	: m_controlPoints(controlPoints)
{
	if (m_controlPoints.size() < 2)
	{
		m_controlPoints = { ImVec2{0, 0}, ImVec2{1, 1} };
		return;
	}
	for (const ImVec2& pt : controlPoints)
	{
		if (m_VisibleRangeLeftBottom.x > pt.x)
			m_VisibleRangeLeftBottom.x = pt.x;
		if (m_VisibleRangeLeftBottom.y > pt.y)
			m_VisibleRangeLeftBottom.y = pt.y;
		if (m_VisibleRangeRightTop.x < pt.x)
			m_VisibleRangeRightTop.x = pt.x;
		if (m_VisibleRangeRightTop.y < pt.y)
			m_VisibleRangeRightTop.y = pt.y;
	}
	SortPointsByX(m_controlPoints);
}
float BlenderCurveEditor::Evaluate(float x)
{
	return EvaluateCurve(m_controlPoints, x);
}
std::vector<float> BlenderCurveEditor::Samples(size_t numSamples)
{
	std::vector<float> result;
	this->Samples(numSamples, result);
	return result;
}
void BlenderCurveEditor::Samples(size_t numSamples, std::vector<float>& resultOut)
{
	EvaluateCurveSamples(m_controlPoints, numSamples, resultOut);
}


namespace ImGui
{
	void ShowColorPicker(ImU32& color, const char* label)
	{
		ImVec4 colorAsVec = ImGui::ColorConvertU32ToFloat4(color);
		if (ImGui::ColorEdit4(
			label, (float*)&colorAsVec))
			color = ImGui::ColorConvertFloat4ToU32(colorAsVec);
	};
}

#include "imgui/imgui_internal.h"
bool BlenderCurveEditor::Draw(ImVec2 editorSize)
{

	std::vector<ImVec2>& controlPoints = m_controlPoints;
	std::optional<size_t>& selectedPoint = m_selectedPoint;

	bool isModified = false;

#if !IMGUI_STYLE_EDIT_SHOW_CURVE_EDITOR_COLOR_PICKERS
	const ImU32 colorBackground = 0xFF515151;
	const ImU32 colorBackgroundGrid = 0xFFABABAB;
	const ImU32 colorControlPoints = 0xFF000000;
	const ImU32 colorSmoothLine = 0xFF000000;
#else
	static ImU32 colorBackground = 0xFF515151;
	static ImU32 colorBackgroundGrid = 0xFFABABAB;
	static ImU32 colorControlPoints = 0xFF000000;
	static ImU32 colorSmoothLine = 0xFF000000;

	ImGui::ShowColorPicker(colorBackground, "colorBackground");
	ImGui::ShowColorPicker(colorBackgroundGrid, "colorBackgroundGrid");
	ImGui::ShowColorPicker(colorControlPoints, "colorControlPoints");
	ImGui::ShowColorPicker(colorSmoothLine, "colorSmoothLine");
#endif


	ImGui::PushID(this);
	ImVec2 visibleRangeX = { m_VisibleRangeLeftBottom.x, m_VisibleRangeRightTop.x };
	ImVec2 visibleRangeY = { m_VisibleRangeLeftBottom.y, m_VisibleRangeRightTop.y };
	{
		const float eps = 0;
		{
			ImGui::PushMultiItemsWidths(2, 250);
			ImGui::DragFloat("##xmin", &visibleRangeX.x, 0.01f, -2, visibleRangeX.y - eps);
			ImGui::SameLine();
			ImGui::PopItemWidth();
			ImGui::DragFloat("Visible Range X", &visibleRangeX.y, 0.01f, visibleRangeX.x + eps, 2);
			ImGui::SameLine();
			ImGui::PopItemWidth();
			if (ImGui::Button("[0, 1]##X"))
				visibleRangeX = { 0, 1 };
			m_VisibleRangeLeftBottom.x = visibleRangeX.x;
			m_VisibleRangeRightTop.x = visibleRangeX.y;
		}
		{
			ImGui::PushMultiItemsWidths(2, 250);
			ImGui::DragFloat("##ymin", &visibleRangeY.x, 0.01f, -2, visibleRangeY.y - eps);
			ImGui::SameLine();
			ImGui::PopItemWidth();
			ImGui::DragFloat("Visible Range Y", &visibleRangeY.y, 0.01f, visibleRangeY.x + eps, 2);
			ImGui::SameLine();
			ImGui::PopItemWidth();
			if (ImGui::Button("[0, 1]##Y"))
				visibleRangeY = { 0, 1 };
			m_VisibleRangeLeftBottom.y = visibleRangeY.x;
			m_VisibleRangeRightTop.y = visibleRangeY.y;
		}
	}
	if (ImGui::Button("Reset points"))
	{
		m_controlPoints = { {0, 0}, {1, 1} };
		m_selectedPoint.reset();
	}
	ImGui::SameLine();
	if (ImGui::Button("Zoom to all points"))
	{
		auto CompareX = [](const ImVec2& lhs, const ImVec2& rhs) { return lhs.x < rhs.x; };
		auto CompareY = [](const ImVec2& lhs, const ImVec2& rhs) { return lhs.y < rhs.y; };
		auto [hasMinX, hasMaxX] = std::minmax_element(m_controlPoints.begin(), m_controlPoints.end(), CompareX);
		auto [hasMinY, hasMaxY] = std::minmax_element(m_controlPoints.begin(), m_controlPoints.end(), CompareY);

		m_VisibleRangeLeftBottom = { hasMinX->x, hasMinY->y };
		m_VisibleRangeRightTop = { hasMaxX->x, hasMaxY->y };
	}
	ImGui::PopID();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	const ImVec2 topLeftScreenSpace = ImGui::GetCursorScreenPos();
	draw_list->PushClipRect(topLeftScreenSpace, topLeftScreenSpace + editorSize, true);

	auto FindPointIndex = [&](const ImVec2& pt) -> std::optional<size_t> {
		for (size_t i = 0; i < controlPoints.size(); i++)
		{
			if (controlPoints[i].x == pt.x
				&& controlPoints[i].y == pt.y)
			{
				return i;
			}
		}
		return {};
	};
	ImVec2 visibleRangeSize = m_VisibleRangeRightTop - m_VisibleRangeLeftBottom;
	auto PointToWidgetScreenCoords = [&](const ImVec2& point)
	{
		return ImVec2(
			(point.x - m_VisibleRangeLeftBottom.x) / visibleRangeSize.x,
			1 - ((point.y - m_VisibleRangeLeftBottom.y) / visibleRangeSize.y)
		) * editorSize + topLeftScreenSpace;
	};
	auto PointFromWidgetScreenCoords = [&](const ImVec2& pointOnScreen) {
		ImVec2 t = (pointOnScreen - topLeftScreenSpace) / editorSize;
		t.y = 1 - t.y;
		return t * visibleRangeSize + m_VisibleRangeLeftBottom;
	};

	// Draw background square.
	{
		ImVec2 rectMin = topLeftScreenSpace;
		ImVec2 rectMax = topLeftScreenSpace + editorSize;
		draw_list->AddRectFilled(rectMin, rectMax, colorBackground);
	}
	// Draw background grid.
	{
		const ImU32 colorBackgroundGrid_integers = IM_COL32(0xCC, 0xCC, 0xCC, 0xFF);
		const float backgroundGridThickness_integers = 1.3f;
		const float backgroundGridThickness_quarters = 0.5f;
		const float eps = 0.001f;
		auto DrawVerticalLinesEveryStep = [&](float step, ImU32 color, float thickness)
		{
			float fx = m_VisibleRangeLeftBottom.x - fmod(m_VisibleRangeLeftBottom.x, step);
			if (fx < m_VisibleRangeLeftBottom.x + eps)
				fx += step;
			while (fx < m_VisibleRangeRightTop.x)
			{
				draw_list->AddLine(
					PointToWidgetScreenCoords({ fx, m_VisibleRangeLeftBottom.y }),
					PointToWidgetScreenCoords({ fx, m_VisibleRangeRightTop.y }),
					color
					, thickness
				);
				fx += step;
			}
		};
		auto DrawHorizontalLinesEveryStep = [&](float step, ImU32 color, float thickness)
		{
			float fy = m_VisibleRangeLeftBottom.y - fmod(m_VisibleRangeLeftBottom.y, step);
			if (fy < m_VisibleRangeLeftBottom.y + eps)
				fy += step;
			while (fy < m_VisibleRangeRightTop.y)
			{
				draw_list->AddLine(
					PointToWidgetScreenCoords({ m_VisibleRangeLeftBottom.x, fy }),
					PointToWidgetScreenCoords({ m_VisibleRangeRightTop.x, fy }),
					color
					, thickness
				);
				fy += step;
			}
		};
		DrawVerticalLinesEveryStep(0.25f, colorBackgroundGrid, backgroundGridThickness_quarters);
		DrawVerticalLinesEveryStep(1.0f, colorBackgroundGrid_integers, backgroundGridThickness_integers);
		DrawHorizontalLinesEveryStep(0.25f, colorBackgroundGrid, backgroundGridThickness_quarters);
		DrawHorizontalLinesEveryStep(1.0f, colorBackgroundGrid_integers, backgroundGridThickness_integers);
		const ImU32 colorBackgroundGrid_axes = IM_COL32(0xCC, 0x44, 0x44, 0xFF);;
		const float backgroundGridThickness_axes = 1.3f;
		if (m_VisibleRangeLeftBottom.x + eps < 0)
		{
			draw_list->AddLine(
				PointToWidgetScreenCoords({ 0, m_VisibleRangeLeftBottom.y }),
				PointToWidgetScreenCoords({ 0, m_VisibleRangeRightTop.y }),
				colorBackgroundGrid_axes
				, backgroundGridThickness_axes
			);
		}
		if (m_VisibleRangeLeftBottom.y + eps < 0)
		{
			draw_list->AddLine(
				PointToWidgetScreenCoords({ m_VisibleRangeLeftBottom.x, 0 }),
				PointToWidgetScreenCoords({ m_VisibleRangeRightTop.x, 0 }),
				colorBackgroundGrid_axes
				, backgroundGridThickness_axes
			);
		}
	}

	const ImVec2 pointMarkerRadius = { 2, 2 };
	// Extracted for convenient debugging.
	auto DrawPoint = [&](ImVec2 pt) {
		pt = PointToWidgetScreenCoords(pt);
		ImVec2 pt_min = pt - pointMarkerRadius;
		ImVec2 pt_max = pt + pointMarkerRadius;
		draw_list->AddRect(pt_min, pt_max, colorControlPoints);
	};

	// Draw a smooth curve.
	CurveEvaluationHelper smoothSamples{ controlPoints, m_VisibleRangeLeftBottom.x, m_VisibleRangeRightTop.x };
	for (int i = 0; i < smoothSamples.equidistantSamples.size() - 1; ++i)
	{
		draw_list->AddLine(
			PointToWidgetScreenCoords(smoothSamples.equidistantSamples[i]),
			PointToWidgetScreenCoords(smoothSamples.equidistantSamples[(size_t)i + 1]),
			colorSmoothLine);
	}

	// Draw the Control Points.
	for (ImVec2 pt : controlPoints)
	{
		DrawPoint(pt);
	}

	// Handle mouse input.
	{
		ImVec2 mousePos = ImGui::GetMousePos();
		// Evaluation test.
		{
			float currentX = PointFromWidgetScreenCoords(mousePos).x;
			if (currentX >= 0 && currentX <= 1.0f)
			{
				DrawPoint({ currentX, smoothSamples.Evaluate(currentX) });
			}
		}
		bool isClickedConfirmed = ImGui::InvisibleButton("Custom curve editor invisible button", editorSize);
		bool isJustClicked = ImGui::IsItemClicked(0);
		bool isJustReleased = ImGui::IsMouseReleased(0);

		if (isJustReleased)
		{
			// Release the selection when mouse is released.
			// Check if the selection points are close enough to be merged.
			// Only immediate neighbors may be merged.
			if (selectedPoint)
			{
				const float requiredClosenessForMergePoints = 36;
				ImVec2 selectedPointScreenSpace = PointToWidgetScreenCoords(controlPoints[selectedPoint.value()]);
				// Potential merge on the right side (doesn't apply to rightmost point).
				if (selectedPoint.value() < controlPoints.size() - 1)
				{
					ImVec2 vectorToRightNeighbour = PointToWidgetScreenCoords(controlPoints[selectedPoint.value() + 1]) - selectedPointScreenSpace;
					float distanceToRightNeighbourSqr =
						vectorToRightNeighbour.x * vectorToRightNeighbour.x +
						vectorToRightNeighbour.y * vectorToRightNeighbour.y;
					if (distanceToRightNeighbourSqr <= requiredClosenessForMergePoints)
					{
						controlPoints.erase(std::next(controlPoints.begin(), selectedPoint.value()));
						isModified = true;
					}
				}
				// Potential merge on the left side (doesn't apply to leftmost point).
				if (selectedPoint.value() > 0)
				{
					ImVec2 vectorToLeftNeighbour = PointToWidgetScreenCoords(controlPoints[selectedPoint.value() - 1]) - selectedPointScreenSpace;
					float distanceToLeftNeighbourSqr =
						vectorToLeftNeighbour.x * vectorToLeftNeighbour.x +
						vectorToLeftNeighbour.y * vectorToLeftNeighbour.y;
					if (distanceToLeftNeighbourSqr <= requiredClosenessForMergePoints)
					{
						controlPoints.erase(std::next(controlPoints.begin(), selectedPoint.value() - 1));
						isModified = true;
					}
				}
			}
			selectedPoint.reset();
		}
		if (isJustClicked)
		{
			// If mouse is clicked close enough to one of the control points,
			// select and grab that one.
			{
				size_t bestMatchIdx = 0;
				float bestDistanceToMouseSqr = std::numeric_limits<float>::infinity();
				const float requiredClosenessToMouse = 400;
				for (size_t i = 0; i < controlPoints.size(); i++)
				{
					ImVec2 pt = PointToWidgetScreenCoords(controlPoints[i]);
					ImVec2 vecToMouse = mousePos - pt;
					float distanceToMouseSqr =
						vecToMouse.x * vecToMouse.x +
						vecToMouse.y * vecToMouse.y;
					if (distanceToMouseSqr <= bestDistanceToMouseSqr)
					{
						bestDistanceToMouseSqr = distanceToMouseSqr;
						bestMatchIdx = i;
					}
				}
				if (bestDistanceToMouseSqr <= requiredClosenessToMouse)
				{
					selectedPoint = bestMatchIdx;
				}
			}
			// If nothing was grabbed with a click, place a new point and grab that.
			if (!selectedPoint)
			{
				ImVec2 newPoint = PointFromWidgetScreenCoords(mousePos);
				controlPoints.push_back(newPoint);
				SortPointsByX(controlPoints);
				isModified = true;
				selectedPoint = FindPointIndex(newPoint);
			}
		}
		// Move the grabbed point.
		if (selectedPoint)
		{
			ImVec2 movedPoint = PointFromWidgetScreenCoords(mousePos);
			auto ClampPointToVisibleRange = [&](ImVec2& pt)
			{
				if (pt.x < m_VisibleRangeLeftBottom.x)
					pt.x = m_VisibleRangeLeftBottom.x;
				if (pt.x > m_VisibleRangeRightTop.x)
					pt.x = m_VisibleRangeRightTop.x;
				if (pt.y < m_VisibleRangeLeftBottom.y)
					pt.y = m_VisibleRangeLeftBottom.y;
				if (pt.y > m_VisibleRangeRightTop.y)
					pt.y = m_VisibleRangeRightTop.y;
			};
			ClampPointToVisibleRange(movedPoint);
			//movedPoint = Clamp_01(movedPoint);
			controlPoints[selectedPoint.value()] = movedPoint;
			isModified = true;

			// Sort points after move.
			{
				SortPointsByX(controlPoints);
				// What was just a selected point might have moved after sort. Find the new index.
				selectedPoint = FindPointIndex(movedPoint);
			}
		}
	}
	draw_list->PopClipRect();
	return isModified;
}
void BlenderCurveEditor::GetControlPointsCopy(std::vector<ImVec2>& resultOut)
{
	resultOut = m_controlPoints;
}
