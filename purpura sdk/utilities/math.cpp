#include "math.h"

vec3_t global_math::vector_transform(vec3_t vector, matrix3x4_t matrix) {
	return vec3_t(vector.Dot2(matrix[0]) + matrix[0][3], vector.Dot2(matrix[1]) + matrix[1][3], vector.Dot2(matrix[2]) + matrix[2][3]);
}

double global_math::deg2rad(double deg) {
	return deg * pi_aprox / 180.;
}

int global_math::clamp_int(int value, int min, int max) {
	if (value < min)
		value = min;
	if (value > max)
		value = max;
	return value;
}

float global_math::clamp_float(float value, float min, float max) {
	if (value < min)
		value = min;
	if (value > max)
		value = max;
	return value;
}

void global_math::normalize_angles(vec3_t& angles) {
	for (auto i = 0; i < 3; i++) {
		while (angles[i] < -180.0f) angles[i] += 360.0f;
		while (angles[i] > 180.0f) angles[i] -= 360.0f;
	}
}

void global_math::vector_angles2(const vec3_t& forward, vec3_t& up, vec3_t& angles) {
	vec3_t left = CrossProduct(up, forward);
	left.NormalizeInPlace();

	float forwardDist = forward.Length2D();

	if (forwardDist > 0.001f)
	{
		angles.x = atan2f(-forward.z, forwardDist) * 180 / pi_aprox;
		angles.y = atan2f(forward.y, forward.x) * 180 / pi_aprox;

		float upZ = (left.y * forward.x) - (left.x * forward.y);
		angles.z = atan2f(left.z, upZ) * 180 / pi_aprox;
	}
	else
	{
		angles.x = atan2f(-forward.z, forwardDist) * 180 / pi_aprox;
		angles.y = atan2f(-left.x, left.y) * 180 / pi_aprox;
		angles.z = 0;
	}
}

void global_math::vector_angles(const vec3_t& forward, vec3_t& angles) {
	float tmp, yaw, pitch;

	if (forward[1] == 0 && forward[0] == 0) {
		yaw = 0;
		if (forward[2] > 0)
			pitch = 270;
		else
			pitch = 90;
	}
	else {
		yaw = (atan2(forward[1], forward[0]) * 180 / pi_aprox);
		if (yaw < 0)
			yaw += 360;

		tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
		pitch = (atan2(-forward[2], tmp) * 180 / pi_aprox);
		if (pitch < 0)
			pitch += 360;
	}

	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}

void inline sin_cos(float radians, float* sine, float* cosine) {
	*sine = sin(radians);
	*cosine = cos(radians);
}

vec3_t global_math::calculate_angle(const vec3_t& src, const vec3_t& dst) {
	vec3_t ret;
	global_math::vector_angles(dst - src, ret);
	return ret;
}

void global_math::angle_vectors(const vec3_t& angles, vec3_t* forward, vec3_t* right, vec3_t* up) {
	float sr, sp, sy, cr, cp, cy;

	sin_cos(deg2rad(angles[1]), &sy, &cy);
	sin_cos(deg2rad(angles[0]), &sp, &cp);
	sin_cos(deg2rad(angles[2]), &sr, &cr);

	if (forward) {
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}

	if (right) {
		right->x = (-1 * sr * sp * cy + -1 * cr * -sy);
		right->y = (-1 * sr * sp * sy + -1 * cr * cy);
		right->z = -1 * sr * cp;
	}

	if (up) {
		up->x = (cr * sp * cy + -sr * -sy);
		up->y = (cr * sp * sy + -sr * cy);
		up->z = cr * cp;
	}
}

void global_math::correct_movement(c_usercmd* cmd, vec3_t wishangle) {
	vec3_t view_fwd, view_right, view_up, cmd_fwd, cmd_right, cmd_up;
	angle_vectors(wishangle, &view_fwd, &view_right, &view_up);
	angle_vectors(cmd->viewangles, &cmd_fwd, &cmd_right, &cmd_up);

	const auto v8 = sqrtf((view_fwd.x * view_fwd.x) + (view_fwd.y * view_fwd.y));
	const auto v10 = sqrtf((view_right.x * view_right.x) + (view_right.y * view_right.y));
	const auto v12 = sqrtf(view_up.z * view_up.z);

	const vec3_t norm_view_fwd((1.f / v8) * view_fwd.x, (1.f / v8) * view_fwd.y, 0.f);
	const vec3_t norm_view_right((1.f / v10) * view_right.x, (1.f / v10) * view_right.y, 0.f);
	const vec3_t norm_view_up(0.f, 0.f, (1.f / v12) * view_up.z);

	const auto v14 = sqrtf((cmd_fwd.x * cmd_fwd.x) + (cmd_fwd.y * cmd_fwd.y));
	const auto v16 = sqrtf((cmd_right.x * cmd_right.x) + (cmd_right.y * cmd_right.y));
	const auto v18 = sqrtf(cmd_up.z * cmd_up.z);

	const vec3_t norm_cmd_fwd((1.f / v14) * cmd_fwd.x, (1.f / v14) * cmd_fwd.y, 0.f);
	const vec3_t norm_cmd_right((1.f / v16) * cmd_right.x, (1.f / v16) * cmd_right.y, 0.f);
	const vec3_t norm_cmd_up(0.f, 0.f, (1.f / v18) * cmd_up.z);

	const auto v22 = norm_view_fwd.x * cmd->forwardmove;
	const auto v26 = norm_view_fwd.y * cmd->forwardmove;
	const auto v28 = norm_view_fwd.z * cmd->forwardmove;
	const auto v24 = norm_view_right.x * cmd->sidemove;
	const auto v23 = norm_view_right.y * cmd->sidemove;
	const auto v25 = norm_view_right.z * cmd->sidemove;
	const auto v30 = norm_view_up.x * cmd->upmove;
	const auto v27 = norm_view_up.z * cmd->upmove;
	const auto v29 = norm_view_up.y * cmd->upmove;

	cmd->forwardmove = ((((norm_cmd_fwd.x * v24) + (norm_cmd_fwd.y * v23)) + (norm_cmd_fwd.z * v25))
		+ (((norm_cmd_fwd.x * v22) + (norm_cmd_fwd.y * v26)) + (norm_cmd_fwd.z * v28)))
		+ (((norm_cmd_fwd.y * v30) + (norm_cmd_fwd.x * v29)) + (norm_cmd_fwd.z * v27));
	cmd->sidemove = ((((norm_cmd_right.x * v24) + (norm_cmd_right.y * v23)) + (norm_cmd_right.z * v25))
		+ (((norm_cmd_right.x * v22) + (norm_cmd_right.y * v26)) + (norm_cmd_right.z * v28)))
		+ (((norm_cmd_right.x * v29) + (norm_cmd_right.y * v30)) + (norm_cmd_right.z * v27));
	cmd->upmove = ((((norm_cmd_up.x * v23) + (norm_cmd_up.y * v24)) + (norm_cmd_up.z * v25))
		+ (((norm_cmd_up.x * v26) + (norm_cmd_up.y * v22)) + (norm_cmd_up.z * v28)))
		+ (((norm_cmd_up.x * v30) + (norm_cmd_up.y * v29)) + (norm_cmd_up.z * v27));

	const auto ratio = 2.f - fmaxf(fabsf(cmd->sidemove), fabsf(cmd->forwardmove)) / 450.f;
	cmd->forwardmove *= ratio;
	cmd->sidemove *= ratio;

	//wishangle = cmd->viewangles;

	/*float delta_view;
	float f1;
	float f2;

	if (old_angles.y < 0.f)
		f1 = 360.0f + old_angles.y;
	else
		f1 = old_angles.y;

	if (cmd->viewangles.y < 0.0f)
		f2 = 360.0f + cmd->viewangles.y;
	else
		f2 = cmd->viewangles.y;

	if (f2 < f1)
		delta_view = abs(f2 - f1);
	else
		delta_view = 360.0f - abs(f1 - f2);

	delta_view = 360.0f - delta_view;

	cmd->forwardmove = cos(DEG2RAD(delta_view)) * old_forwardmove + cos(DEG2RAD(delta_view + 90.f)) * old_sidemove;
	cmd->sidemove = sin(DEG2RAD(delta_view)) * old_forwardmove + sin(DEG2RAD(delta_view + 90.f)) * old_sidemove;*/
}

float global_math::normalize_yaw(float& yaw)
{
	while (yaw < 180.0f) yaw += 360.0f;
	while (yaw > 180.0f) yaw -= 360.0f;
	return yaw;
}