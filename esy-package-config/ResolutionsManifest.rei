type t

and resolution = {
  name: string,
  source,
  override: option(Json.t),
}

and source =
  | OfVersionSpec(VersionSpec.t)
  | OfLink(Link.t);

let empty: t;
let elements: t => list(resolution);

let of_yojson: Json.decoder(t);
