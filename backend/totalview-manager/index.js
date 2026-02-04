import express from "express";
import { InstancesClient } from "@google-cloud/compute";

const app = express();

// CORS: allow frontend origins (required when API key is sent; * doesn't work with credentials)
const ALLOWED_ORIGINS = (process.env.ALLOWED_ORIGINS || "http://localhost:5173,http://localhost:3000").split(",").map((s) => s.trim());
app.use((req, res, next) => {
  const origin = req.get("origin");
  if (origin && ALLOWED_ORIGINS.includes(origin)) {
    res.setHeader("Access-Control-Allow-Origin", origin);
  }
  res.setHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  res.setHeader("Access-Control-Allow-Headers", "Content-Type, x-api-key");
  if (req.method === "OPTIONS") {
    return res.sendStatus(204);
  }
  next();
});

app.use(express.json());

const compute = new InstancesClient();

// REQUIRED env vars (set in Cloud Run)
const PROJECT_ID = process.env.PROJECT_ID;
const ZONE = process.env.ZONE;
const INSTANCE_NAME = process.env.INSTANCE_NAME;

// Simple PoC auth
const API_KEY = process.env.API_KEY;

function requireApiKey(req, res) {
  const provided = req.get("x-api-key");
  if (!API_KEY || provided !== API_KEY) {
    res.status(401).json({ ok: false, error: "unauthorized" });
    return false;
  }
  return true;
}

function requireConfig(res) {
  if (!PROJECT_ID || !ZONE || !INSTANCE_NAME) {
    res.status(500).json({
      ok: false,
      error: "Missing env vars: PROJECT_ID, ZONE, INSTANCE_NAME",
    });
    return false;
  }
  return true;
}

// Health / sanity endpoint
app.get("/", (req, res) => {
  res.json({ ok: true, service: "vm-control" });
});

// Start VM
app.post("/start", async (req, res) => {
  try {
    if (!requireApiKey(req, res)) return;
    if (!requireConfig(res)) return;

    await compute.start({
      project: PROJECT_ID,
      zone: ZONE,
      instance: INSTANCE_NAME,
    });

    res.json({ ok: true, action: "starting" });
  } catch (err) {
    console.error(err);
    res.status(500).json({ ok: false, error: String(err?.message || err) });
  }
});

// Stop VM
app.post("/stop", async (req, res) => {
  try {
    if (!requireApiKey(req, res)) return;
    if (!requireConfig(res)) return;

    await compute.stop({
      project: PROJECT_ID,
      zone: ZONE,
      instance: INSTANCE_NAME,
    });

    res.json({ ok: true, action: "stopping" });
  } catch (err) {
    console.error(err);
    res.status(500).json({ ok: false, error: String(err?.message || err) });
  }
});

// VM status
app.get("/status", async (req, res) => {
  try {
    if (!requireApiKey(req, res)) return;
    if (!requireConfig(res)) return;

    const [inst] = await compute.get({
      project: PROJECT_ID,
      zone: ZONE,
      instance: INSTANCE_NAME,
    });

    res.json({
      ok: true,
      status: inst.status, // RUNNING, TERMINATED, STOPPING, PROVISIONING, etc.
      name: inst.name,
      zone: ZONE,
    });
  } catch (err) {
    console.error(err);
    res.status(500).json({ ok: false, error: String(err?.message || err) });
  }
});

// Catch-all
app.use((req, res) => {
  res.status(404).json({ ok: false, error: "not found" });
});

const port = parseInt(process.env.PORT || "8080", 10);
app.listen(port, () => {
  console.log(`vm-control: listening on port ${port}`);
});
