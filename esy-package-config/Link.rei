type t = {
  path: DistPath.t,
  manifest: option(ManifestSpec.t),
  kind,
}

and kind =
  | LinkRegular
  | LinkDev;

include S.COMPARABLE with type t := t;

let show: t => string;
let pp: Fmt.t(t);

let showPretty: t => string;
let ppPretty: Fmt.t(t);

let parser: Parse.t(t);

let of_yojson: Json.decoder(t);
let sexp_of_t: t => Sexplib0.Sexp.t;
