#define NEUTRAL_NATURE      \
    .statUpId = NUM_STATS,  \
	.statDownId = NUM_STATS

const struct NatureInfo gNaturesInfo[NUM_NATURES] = 
{
    [NATURE_HARDY] =
	{
		.name = COMPOUND_STRING("Hardy"),
		NEUTRAL_NATURE,
	},
    [NATURE_LONELY] =
	{
		.name = COMPOUND_STRING("Lonely"),
		.statUpId = STAT_ATK,
		.statDownId = STAT_DEF,
	},
    [NATURE_BRAVE] =
	{
		.name = COMPOUND_STRING("Brave"),
		.statUpId = STAT_ATK,
		.statDownId = STAT_SPEED,
	},
    [NATURE_ADAMANT] =
	{
		.name = COMPOUND_STRING("Adamant"),
		.statUpId = STAT_ATK,
		.statDownId = STAT_SPATK,
	},
    [NATURE_NAUGHTY] =
	{
		.name = COMPOUND_STRING("Naughty"),
		.statUpId = STAT_ATK,
		.statDownId = STAT_SPDEF,
	},
    [NATURE_BOLD] =
	{
		.name = COMPOUND_STRING("Bold"),
		.statUpId = STAT_DEF,
		.statDownId = STAT_ATK,
	},
    [NATURE_DOCILE] =
	{
		.name = COMPOUND_STRING("Docile"),
		NEUTRAL_NATURE,
	},
    [NATURE_RELAXED] =
	{
		.name = COMPOUND_STRING("Relaxed"),
		.statUpId = STAT_DEF,
		.statDownId = STAT_SPEED,
	},
    [NATURE_IMPISH] =
	{
		.name = COMPOUND_STRING("Impish"),
		.statUpId = STAT_DEF,
		.statDownId = STAT_SPATK,
	},
    [NATURE_LAX] =
	{
		.name = COMPOUND_STRING("Lax"),
		.statUpId = STAT_DEF,
		.statDownId = STAT_SPDEF,
	},
    [NATURE_TIMID] =
	{
		.name = COMPOUND_STRING("Timid"),
		.statUpId = STAT_SPEED,
		.statDownId = STAT_ATK,
	},
    [NATURE_HASTY] =
	{
		.name = COMPOUND_STRING("Hasty"),
		.statUpId = STAT_SPEED,
		.statDownId = STAT_DEF,
	},
    [NATURE_SERIOUS] =
	{
		.name = COMPOUND_STRING("Serious"),
		NEUTRAL_NATURE,
	},
    [NATURE_JOLLY] =
	{
		.name = COMPOUND_STRING("Jolly"),
		.statUpId = STAT_SPEED,
		.statDownId = STAT_SPATK,
	},
    [NATURE_NAIVE] =
	{
		.name = COMPOUND_STRING("Naive"),
		.statUpId = STAT_SPEED,
		.statDownId = STAT_SPDEF,
	},
    [NATURE_MODEST] =
	{
		.name = COMPOUND_STRING("Modest"),
		.statUpId = STAT_SPATK,
		.statDownId = STAT_ATK,
	},
    [NATURE_MILD] =
	{
		.name = COMPOUND_STRING("Mild"),
		.statUpId = STAT_SPATK,
		.statDownId = STAT_DEF,
	},
    [NATURE_QUIET] =
	{
		.name = COMPOUND_STRING("Quiet"),
		.statUpId = STAT_SPATK,
		.statDownId = STAT_SPEED,
	},
    [NATURE_BASHFUL] =
	{
		.name = COMPOUND_STRING("Bashful"),
		NEUTRAL_NATURE,
	},
    [NATURE_RASH] =
	{
		.name = COMPOUND_STRING("Rash"),
		.statUpId = STAT_SPATK,
		.statDownId = STAT_SPDEF,
	},
    [NATURE_CALM] =
	{
		.name = COMPOUND_STRING("Calm"),
		.statUpId = STAT_SPDEF,
		.statDownId = STAT_ATK,
	},
    [NATURE_GENTLE] =
	{
		.name = COMPOUND_STRING("Gentle"),
		.statUpId = STAT_SPDEF,
		.statDownId = STAT_DEF,
	},
    [NATURE_SASSY] =
	{
		.name = COMPOUND_STRING("Sassy"),
		.statUpId = STAT_SPDEF,
		.statDownId = STAT_SPEED,
	},
    [NATURE_CAREFUL] =
	{
		.name = COMPOUND_STRING("Careful"),
		.statUpId = STAT_SPDEF,
		.statDownId = STAT_SPATK,
	},
    [NATURE_QUIRKY] =
	{
		.name = COMPOUND_STRING("Quirky"),
		NEUTRAL_NATURE,
	},
};
