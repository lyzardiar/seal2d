local SDL_enum = {
	SDL_FLIP_NONE 		= 0x00000000,     	-- /**< Do not flip */
    SDL_FLIP_HORIZONTAL = 0x00000001, 		-- /**< flip horizontally */
    SDL_FLIP_VERTICAL 	= 0x00000002,     	-- /**< flip vertically */

    SDL_MOUSEMOTION    	= 0x400, 		-- /**< Mouse moved */
    SDL_MOUSEBUTTONDOWN = 0x401,        -- /**< Mouse button pressed */
    SDL_MOUSEBUTTONUP 	= 0x402,        -- /**< Mouse button released */
    SDL_MOUSEWHEEL 		= 0x403,        -- /**< Mouse wheel motion */

    SDL_FINGERDOWN      = 0x700,
    SDL_FINGERUP		= 0x701,
    SDL_FINGERMOTION	= 0x702,

    SDL_DOLLARGESTURE   = 0x800,
    SDL_DOLLARRECORD    = 0x801,
    SDL_MULTIGESTURE    = 0x802,
}

return SDL_enum